#include "Game/__internalConfig/LoggerConfig.h"
#include "ClashSystem.h"
#include <cmath>
#include "Base/IO/Readers/IniReader.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/Inputs/InputContextManager.h"
#include "../BattleEffects.h"
#include "../BattleHit.h"
#include "../BattleRules.h"
#include "../SkillsHolderComponent.h"

ClashSystem::ClashSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, BattleRules& rules, BattleStage& stage, unsigned int blockSize) :
	System(em),
	SubObserver<ska::InputKeyEvent>(std::bind(&ClashSystem::onKeyInputEvent, this, std::placeholders::_1), ged),
	m_entities(em),
	m_ged(ged),
	m_rules(rules),
	m_stage(stage),
	m_blockSize(static_cast<float>(blockSize)) {
}

bool ClashSystem::standing(ska::EntityId entity) const {
	const auto& processed = getEntities();
	if (processed.find(entity) == processed.end()) {
		return false;
	}
	const auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
	return !fighter.knockedOut && !fighter.inBall && m_componentAccessor.get<BattleComponent>(entity).hp > 0;
}

ska::Point<float> ClashSystem::center(ska::EntityId entity) const {
	return ska::PositionComponent::getCenterPosition(m_entities.getComponent<ska::PositionComponent>(entity), m_entities.getComponent<ska::HitboxComponent>(entity));
}

int ClashSystem::attacking(ska::EntityId entity) const {
	const auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
	if (fighter.windUp > 0 && fighter.pendingSkill >= 0) {
		return fighter.pendingSkill;
	}
	if (fighter.lunge > 0) {
		// The skill used last
		auto last = 0;
		for (auto i = 1; i < static_cast<int>(fighter.lastUses.size()); i++) {
			if (fighter.lastUses[static_cast<std::size_t>(i)] > fighter.lastUses[static_cast<std::size_t>(last)]) {
				last = i;
			}
		}
		return last;
	}
	return -1;
}

int ClashSystem::strongest(ska::EntityId entity) const {
	if (!m_entities.hasComponent<SkillsHolderComponent>(entity)) {
		return 0;
	}
	const auto& skills = m_entities.getComponent<SkillsHolderComponent>(entity).skills;
	auto best = 0;
	for (auto i = 1; i < static_cast<int>(skills.size()); i++) {
		if (skills[static_cast<std::size_t>(i)].power > skills[static_cast<std::size_t>(best)].power) {
			best = i;
		}
	}
	return best;
}

std::string ClashSystem::who(ska::EntityId entity) const {
	const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(m_componentAccessor.get<BattleComponent>(entity).monster) + ".ini" };
	const auto name = data.exists("Description name") ? data.get<std::string>("Description name") : "?";
	return m_componentAccessor.get<FighterComponent>(entity).team == 0 ? name : name + " sauvage";
}

bool ClashSystem::onKeyInputEvent(ska::InputKeyEvent& event) {
	auto& turn = m_stage.turn;
	if (!turn.active() || turn.step != BattleTurn::Step::Menu || !standing(m_stage.player) || m_componentAccessor.get<FighterComponent>(m_stage.player).controlledByAI) {
		return true;
	}
	const auto& actions = event.icm.getActions();
	const ska::InputAction skillActions[] = { ska::InputAction::ShotSkill1, ska::InputAction::ShotSkill2, ska::InputAction::ShotSkill3, ska::InputAction::ShotSkill4 };
	for (auto index = 0u; index < 4 && index < turn.skills.size(); index++) {
		if (actions[skillActions[index]]) {
			choose(index);
			return true;
		}
	}
	if (m_choice.keys(event.icm)) {
		choose(m_choice.selected());
	}
	turn.selected = m_choice.selected();
	return true;
}

void ClashSystem::watch() {
	const auto player = m_stage.player;
	if (!standing(player)) {
		return;
	}
	auto& fighter = m_componentAccessor.get<FighterComponent>(player);
	const auto opponent = fighter.opponent;
	const auto ready = !m_hadTurn || m_time - m_lastTurn >= m_rules.turnCooldown();

	// An opening : its dash starts when the attack of an opponent is about to go
	const auto dashing = fighter.dash > 0;
	if (dashing && !m_dashing && ready) {
		for (const auto entity : getEntities()) {
			if (!standing(entity) || m_componentAccessor.get<FighterComponent>(entity).team == fighter.team) {
				continue;
			}
			const auto& enemy = m_componentAccessor.get<FighterComponent>(entity);
			const auto skill = attacking(entity);
			if (skill < 0 || !m_entities.hasComponent<SkillsHolderComponent>(entity)) {
				continue;
			}
			const auto reach = static_cast<float>(m_entities.getComponent<SkillsHolderComponent>(entity).skills[static_cast<std::size_t>(skill)].range) + m_blockSize;
			const auto toPlayer = center(player) - center(entity);
			const auto left = enemy.lunge > 0 ? 0 : static_cast<int>(enemy.windUp);
			if (std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y) <= reach && m_rules.opening(left)) {
				m_dashing = dashing;
				start(BattleTurn::Kind::Opening, entity, -1);
				return;
			}
		}
	}
	m_dashing = dashing;

	// A clash : both attack each other at once (decided once for these two attacks)
	const auto mine = attacking(player);
	const auto theirs = standing(opponent) ? attacking(opponent) : -1;
	const auto both = mine >= 0 && theirs >= 0 && m_componentAccessor.get<FighterComponent>(opponent).opponent == player;
	if (both && !m_bothAttacking && ready) {
		const auto between = center(opponent) - center(player);
		const auto distance = static_cast<int>(std::sqrt(between.x * between.x + between.y * between.y) / m_blockSize);
		if (m_rules.clashes(distance, ska::NumberUtils::random(0, 99))) {
			m_bothAttacking = both;
			start(BattleTurn::Kind::Clash, opponent, theirs);
			return;
		}
	}
	m_bothAttacking = both;
}

void ClashSystem::start(BattleTurn::Kind kind, ska::EntityId opponent, int opponentSkill) {
	m_opponent = opponent;
	m_opponentSkill = opponentSkill;
	m_actions.clear();
	m_action = 0;
	m_hit = false;
	// Their attacks stop : the round replaces them
	for (const auto entity : { m_stage.player, opponent }) {
		auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
		fighter.windUp = 0;
		fighter.pendingSkill = -1;
		fighter.lunge = 0;
		fighter.dash = 0;
		fighter.dodgeRequested = false;
		fighter.knockback = {};
	}
	auto& turn = m_stage.turn;
	turn = BattleTurn{};
	turn.kind = kind;
	turn.pokemon = who(m_stage.player);
	if (m_entities.hasComponent<SkillsHolderComponent>(m_stage.player)) {
		for (const auto& skill : m_entities.getComponent<SkillsHolderComponent>(m_stage.player).skills) {
			if (!skill.name.empty()) {
				turn.skills.push_back(skill.name);
			}
		}
	}
	m_choice = Ui::ChoiceList{};
	m_choice.setCount(turn.skills.size());
	m_stage.hitStop(200);
	m_stage.announce(kind == BattleTurn::Kind::Clash ? "CLASH !" : "OUVERTURE !");
	m_stage.sound(BattleSound::StrongHit);
	m_stage.shake(220, 5);
	m_stage.cue(BattleCue{ kind == BattleTurn::Kind::Clash ? "clash" : "opening", "" });
	SLOG(ska::LogLevel::Info) << "Battle : " << (kind == BattleTurn::Kind::Clash ? "a clash" : "an opening") << ", a turn as in the classic battles";
}

void ClashSystem::choose(std::size_t index) {
	auto& turn = m_stage.turn;
	if (turn.step != BattleTurn::Step::Menu || index >= turn.skills.size()) {
		return;
	}
	turn.selected = index;
	const auto mine = Action{ m_stage.player, m_opponent, static_cast<int>(index) };
	if (turn.kind == BattleTurn::Kind::Clash && m_opponentSkill >= 0) {
		const auto theirs = Action{ m_opponent, m_stage.player, m_opponentSkill };
		const auto& speed = m_componentAccessor.get<BattleComponent>(m_stage.player).speed;
		const auto& other = m_componentAccessor.get<BattleComponent>(m_opponent).speed;
		if (m_rules.playerFirst(speed, other, ska::NumberUtils::random(0, 99))) {
			m_actions = { mine, theirs };
		} else {
			m_actions = { theirs, mine };
		}
	} else {
		m_actions = { mine };
	}
	turn.step = BattleTurn::Step::Acting;
	turn.time = 0;
	m_action = 0;
	m_hit = false;
	m_announced = false;
	SLOG(ska::LogLevel::Info) << "Battle : the turn, the player chooses " << turn.skills[index];
}

void ClashSystem::hit(const Action& action) {
	if (!standing(action.attacker) || !m_entities.hasComponent<SkillsHolderComponent>(action.attacker)) {
		return;
	}
	// The whole skill at once (no particles to share its power), it cannot miss
	auto skill = m_entities.getComponent<SkillsHolderComponent>(action.attacker).skills[static_cast<std::size_t>(action.skill)];
	skill.particleNumber = 1;
	auto wave = BattleMark{};
	wave.kind = BattleMark::Kind::Wave;
	wave.position = center(action.attacker);
	wave.value = 40;
	wave.onPlayer = m_componentAccessor.get<FighterComponent>(action.attacker).team == 0;
	m_stage.mark(wave);
	if (skill.kind == SkillKind::Self) {
		ApplySkillEffects(m_entities, m_stage, m_rules, skill, action.attacker, 0);
		return;
	}
	if (!standing(action.target)) {
		return;
	}
	m_componentAccessor.get<FighterComponent>(action.target).invulnerable = 0;
	if (skill.power > 0 || skill.fixedDamage > 0) {
		auto battleHit = BattleHit{};
		battleHit.attacker = action.attacker;
		battleHit.target = action.target;
		battleHit.skill = &skill;
		const auto toTarget = center(action.target) - center(action.attacker);
		const auto length = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
		battleHit.direction = length < 0.001F ? ska::Point<float>{ 1.F, 0.F } : ska::Point<float>{ toTarget.x / length, toTarget.y / length };
		battleHit.impact = center(action.target);
		ApplyBattleHit(m_entities, m_ged, m_rules, m_stage, battleHit);
	} else {
		ApplySkillEffects(m_entities, m_stage, m_rules, skill, action.attacker, action.target);
	}
}

void ClashSystem::end() {
	m_stage.turn = BattleTurn{};
	m_lastTurn = m_time;
	m_hadTurn = true;
	m_actions.clear();
	SLOG(ska::LogLevel::Info) << "Battle : the turn is over, the real time goes on";
}

void ClashSystem::refresh(unsigned int ellapsedTime) {
	m_time += m_stage.delta();
	auto& turn = m_stage.turn;
	if (!turn.active()) {
		if (m_stage.phase() == BattlePhase::Fight && !m_stage.cinematic.active) {
			watch();
		}
		return;
	}
	// The battle ended meanwhile (a K.O.)
	if (m_stage.phase() != BattlePhase::Fight) {
		end();
		return;
	}
	turn.time += ellapsedTime;
	turn.messageAge += ellapsedTime;
	// The battle stays stopped during the turn (the bars drain : real time)
	m_stage.hitStop(120);
	const auto ai = standing(m_stage.player) && m_componentAccessor.get<FighterComponent>(m_stage.player).controlledByAI;
	switch (turn.step) {
	case BattleTurn::Step::Intro:
		if (turn.time >= BattleTurn::IntroDuration) {
			turn.step = BattleTurn::Step::Menu;
			turn.time = 0;
			turn.message = "Que doit faire " + turn.pokemon + " ?";
			turn.messageAge = 0;
		}
		break;
	case BattleTurn::Step::Menu:
		if (ai && turn.time >= 800) {
			choose(static_cast<std::size_t>(strongest(m_stage.player)));
		} else if (turn.time >= BattleTurn::MenuDuration) {
			choose(turn.selected);
		}
		break;
	case BattleTurn::Step::Acting: {
		if (m_action >= m_actions.size()) {
			turn.step = BattleTurn::Step::Outro;
			turn.time = 0;
			break;
		}
		const auto& action = m_actions[m_action];
		if (!m_announced) {
			// Its message, a K.O. skips it
			if (!standing(action.attacker)) {
				m_action++;
				turn.time = 0;
				break;
			}
			m_announced = true;
			const auto& skill = m_entities.getComponent<SkillsHolderComponent>(action.attacker).skills[static_cast<std::size_t>(action.skill)];
			turn.message = who(action.attacker) + " utilise " + skill.name + " !";
			turn.messageAge = 0;
			m_hit = false;
			m_stage.sound(BattleSound::Cast);
		}
		if (!m_hit && turn.time >= BattleTurn::HitDelay) {
			m_hit = true;
			hit(action);
		}
		if (turn.time >= BattleTurn::ActionDuration) {
			m_action++;
			m_announced = false;
			turn.time = 0;
		}
		break;
	}
	case BattleTurn::Step::Outro:
		if (turn.time >= BattleTurn::OutroDuration) {
			end();
		}
		break;
	}
}
