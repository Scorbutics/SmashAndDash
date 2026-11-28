#include "Game/__internalConfig/LoggerConfig.h"
#include "ClashSystem.h"
#include <cmath>
#include "Base/IO/Readers/IniReader.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/Data/Events/SoundEvent.h"
#include "Core/Inputs/InputContextManager.h"
#include "../BattleEffects.h"
#include "../BattleHit.h"
#include "../BattleItems.h"
#include "../../Data/Bag.h"
#include "../../Data/Team.h"
#include "../BattleRules.h"
#include "../SkillsHolderComponent.h"
#include "../../World/WorldState.h"

ClashSystem::ClashSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState, BattleStage& stage, unsigned int blockSize) :
	System(em),
	SubObserver<ska::InputKeyEvent>(std::bind(&ClashSystem::onKeyInputEvent, this, std::placeholders::_1), ged),
	m_entities(em),
	m_ged(ged),
	m_worldState(worldState),
	m_rules(worldState.getBattleRules()),
	m_stage(stage),
	m_blockSize(static_cast<float>(blockSize)),
	m_music("./Music/clash.ogg") {
}

void ClashSystem::leave() {
	if (m_stage.turn.active()) {
		end();
	}
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
	// The strike of a dash skill (its dashes before only place it)
	if (fighter.dashAttack.phase == DashAttack::Phase::Strike) {
		return fighter.dashAttack.skill;
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

int ClashSystem::opponentPick() const {
	if (!m_entities.hasComponent<SkillsHolderComponent>(m_opponent)) {
		return 0;
	}
	auto powers = std::vector<int>{};
	for (const auto& skill : m_entities.getComponent<SkillsHolderComponent>(m_opponent).skills) {
		const auto usable = !skill.name.empty() && skill.particleNumber > 0;
		powers.push_back(usable ? std::max({ skill.power, skill.fixedDamage, 1 }) : 0);
	}
	return m_rules.clashPick(powers, ska::NumberUtils::random(0, 99));
}

std::string ClashSystem::who(ska::EntityId entity) const {
	const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(m_componentAccessor.get<BattleComponent>(entity).monster) + ".ini" };
	return data.exists("Description name") ? data.get<std::string>("Description name") : "?";
}

bool ClashSystem::onKeyInputEvent(ska::InputKeyEvent& event) {
	auto& turn = m_stage.turn;
	if (!turn.active() || turn.step != BattleTurn::Step::Menu || !standing(m_stage.player) || m_componentAccessor.get<FighterComponent>(m_stage.player).controlledByAI) {
		return true;
	}
	const auto& actions = event.icm.getActions();
	if (!turn.bag) {
		const ska::InputAction skillActions[] = { ska::InputAction::ShotSkill1, ska::InputAction::ShotSkill2, ska::InputAction::ShotSkill3, ska::InputAction::ShotSkill4 };
		for (auto index = 0u; index < 4 && index < turn.skills.size(); index++) {
			if (actions[skillActions[index]]) {
				choose(index);
				return true;
			}
		}
	}
	const auto before = m_choice.selected();
	if (m_choice.keys(event.icm)) {
		if (turn.bag) {
			chooseItem(m_choice.selected());
		} else {
			choose(m_choice.selected());
		}
		return true;
	}
	if (m_choice.selected() != before) {
		m_stage.sound(BattleSound::ClashCursor);
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
	// Asked by a script (Battle.clash) : now, with the strongest skill of the opponent
	if (m_stage.clashRequested) {
		m_stage.clashRequested = false;
		if (standing(opponent)) {
			start(BattleTurn::Kind::Clash, opponent, strongest(opponent));
			return;
		}
	}

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
	m_announced = false;
	// Their attacks stop : the rounds replace them
	for (const auto entity : { m_stage.player, opponent }) {
		auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
		fighter.windUp = 0;
		fighter.pendingSkill = -1;
		fighter.lunge = 0;
		fighter.dash = 0;
		fighter.dashAttack = {};
		fighter.dodgeRequested = false;
		fighter.knockback = {};
	}
	auto& turn = m_stage.turn;
	turn = BattleTurn{};
	turn.kind = kind;
	turn.id = ++m_turns;
	turn.player = m_stage.player;
	turn.opponent = opponent;
	const auto mine = center(m_stage.player);
	const auto theirs = center(opponent);
	turn.center = ska::Point<float>{ (mine.x + theirs.x) / 2.F, (mine.y + theirs.y) / 2.F };
	turn.pokemon = who(m_stage.player);
	turn.opponentName = who(opponent);
	turn.rounds = m_rules.clashRounds();
	if (m_entities.hasComponent<SkillsHolderComponent>(m_stage.player)) {
		for (const auto& skill : m_entities.getComponent<SkillsHolderComponent>(m_stage.player).skills) {
			if (!skill.name.empty()) {
				turn.skills.push_back(skill.name);
				turn.skillTypes.push_back(skill.type);
			}
		}
	}
	m_choice = Ui::ChoiceList{};
	m_choice.setCount(turn.skills.size() + 1);
	step(BattleTurn::Step::Impact);
	interface(true);
	m_stage.hitStop(200);
	m_stage.sound(BattleSound::ClashImpact);
	m_stage.sound(BattleSound::StrongHit);
	m_stage.shake(320, 9);
	m_stage.cue(BattleCue{ kind == BattleTurn::Kind::Clash ? "clash" : "opening", "" });
	SLOG(ska::LogLevel::Info) << "Battle : " << (kind == BattleTurn::Kind::Clash ? "a clash" : "an opening") << ", " << turn.rounds << " rounds as in the classic battles";
}

void ClashSystem::step(BattleTurn::Step next) {
	auto& turn = m_stage.turn;
	turn.step = next;
	turn.time = 0;
	switch (next) {
	case BattleTurn::Step::Zoom:
		m_stage.sound(BattleSound::ClashWoosh);
		break;
	case BattleTurn::Step::Shatter:
		m_stage.sound(BattleSound::ClashShatter);
		break;
	case BattleTurn::Step::Versus:
		m_stage.sound(BattleSound::ClashVersus);
		music(true);
		break;
	case BattleTurn::Step::Rebuild:
		m_stage.sound(BattleSound::ClashSwoosh);
		break;
	case BattleTurn::Step::Round:
		m_stage.sound(BattleSound::ClashRound);
		turn.message.clear();
		break;
	case BattleTurn::Step::Menu:
		turn.message = "Que doit faire " + turn.pokemon + " ?";
		turn.messageAge = 0;
		break;
	case BattleTurn::Step::KnockOut:
		m_stage.sound(BattleSound::ClashKnockOut);
		m_stage.sound(BattleSound::KnockOut);
		break;
	case BattleTurn::Step::Return:
		m_stage.sound(BattleSound::ClashWoosh);
		m_stage.sound(BattleSound::ClashShatter);
		turn.message.clear();
		break;
	default:
		break;
	}
}

void ClashSystem::intro(unsigned int before, unsigned int now) {
	const auto crossed = [before, now](unsigned int at) { return before < at && now >= at; };
	switch (m_stage.turn.step) {
	case BattleTurn::Step::Shatter:
		if (crossed(380) || crossed(620)) {
			m_stage.sound(BattleSound::ClashShatter);
		}
		break;
	case BattleTurn::Step::Versus:
		// "VS" lands
		if (crossed(420)) {
			m_stage.sound(BattleSound::StrongHit);
		}
		if (crossed(1250)) {
			m_stage.sound(BattleSound::ClashSwoosh);
		}
		break;
	case BattleTurn::Step::Rebuild:
		// The two platforms land
		if (crossed(700) || crossed(860)) {
			m_stage.sound(BattleSound::ClashLand);
		}
		break;
	case BattleTurn::Step::Round:
		if (crossed(BattleTurn::FightAt)) {
			m_stage.sound(BattleSound::ClashShine);
			m_stage.sound(BattleSound::StrongHit);
		}
		break;
	default:
		break;
	}
}

void ClashSystem::choose(std::size_t index) {
	auto& turn = m_stage.turn;
	if (turn.step != BattleTurn::Step::Menu) {
		return;
	}
	// After the skills : the bag
	if (index == turn.skills.size()) {
		openBag();
		return;
	}
	if (index > turn.skills.size()) {
		return;
	}
	turn.selected = index;
	play(Action{ m_stage.player, m_opponent, static_cast<int>(index), -1 }, false);
	SLOG(ska::LogLevel::Info) << "Battle : the clash, round " << turn.round << ", the player chooses " << turn.skills[index];
}

void ClashSystem::openBag() {
	auto& turn = m_stage.turn;
	auto& bag = m_worldState.getBag();
	turn.itemIds.clear();
	turn.items.clear();
	turn.itemCounts.clear();
	for (const auto item : BattleItems(bag)) {
		const auto data = Bag::data(item);
		turn.itemIds.push_back(item);
		turn.items.push_back(data.has_value() ? data->name : "?");
		turn.itemCounts.push_back(bag.count(item));
	}
	turn.bag = true;
	turn.selected = 0;
	m_choice = Ui::ChoiceList{};
	m_choice.setCount(turn.items.size() + 1);
	turn.message = turn.items.empty() ? "Le sac est vide..." : "Quel objet utiliser ?";
	turn.messageAge = 0;
	m_stage.sound(BattleSound::ClashSelect);
}

void ClashSystem::closeBag() {
	auto& turn = m_stage.turn;
	turn.bag = false;
	m_choice = Ui::ChoiceList{};
	m_choice.setCount(turn.skills.size() + 1);
	turn.selected = turn.skills.size();
	turn.message = "Que doit faire " + turn.pokemon + " ?";
	turn.messageAge = 0;
	m_stage.sound(BattleSound::ClashCursor);
}

void ClashSystem::chooseItem(std::size_t index) {
	auto& turn = m_stage.turn;
	if (turn.step != BattleTurn::Step::Menu || !turn.bag) {
		return;
	}
	// The last choice : back to the skills
	if (index >= turn.itemIds.size()) {
		closeBag();
		return;
	}
	turn.bag = false;
	SLOG(ska::LogLevel::Info) << "Battle : the clash, round " << turn.round << ", the player uses " << turn.items[index];
	// An item goes first (as in the first games)
	play(Action{ m_stage.player, m_opponent, -1, turn.itemIds[index] }, true);
}

void ClashSystem::play(const Action& mine, bool first) {
	auto& turn = m_stage.turn;
	m_stage.sound(BattleSound::ClashSelect);
	// The first round : the skill the opponent was attacking with (a clash), none (an opening) ; then its choice
	auto theirsSkill = turn.round == 1 ? m_opponentSkill : opponentPick();
	if (turn.round == 1 && turn.kind == BattleTurn::Kind::Opening) {
		theirsSkill = -1;
	}
	if (theirsSkill >= 0 && standing(m_opponent)) {
		const auto theirs = Action{ m_opponent, m_stage.player, theirsSkill, -1 };
		const auto& speed = m_componentAccessor.get<BattleComponent>(m_stage.player).speed;
		const auto& other = m_componentAccessor.get<BattleComponent>(m_opponent).speed;
		if (first || m_rules.playerFirst(speed, other, ska::NumberUtils::random(0, 99))) {
			m_actions = { mine, theirs };
		} else {
			m_actions = { theirs, mine };
		}
	} else {
		m_actions = { mine };
	}
	turn.step = BattleTurn::Step::Acting;
	turn.time = 0;
	turn.message.clear();
	m_action = 0;
	m_announced = false;
	m_roundEnd = 0;
	m_flinched = 0;
}

void ClashSystem::actItem(const Action& action, unsigned int before) {
	auto& turn = m_stage.turn;
	const auto crossed = [before, &turn](unsigned int at) { return before < at && turn.time >= at; };
	if (!m_announced) {
		m_announced = true;
		const auto data = Bag::data(action.item);
		const auto name = data.has_value() ? data->name : std::string{ "?" };
		turn.attacker = action.attacker;
		turn.target = action.target;
		turn.skillSprite = -1;
		turn.contact = false;
		turn.item = action.item;
		turn.hit = false;
		turn.damage = 0;
		turn.healed = 0;
		turn.ball = data.has_value() && data->capture;
		turn.caught = false;
		turn.shakes = 0;
		turn.messageAge = 0;
		if (turn.ball) {
			if (Team{ m_worldState.getSaveGame().getPathName() }.size() >= Team::MaxSize || !standing(action.target)) {
				turn.ball = false;
				turn.message = "Ton équipe est pleine !";
				return;
			}
			m_worldState.getBag().remove(action.item, 1);
			const auto dice = RollCapture(m_entities, m_worldState, action.target);
			turn.caught = dice.caught;
			turn.shakes = dice.shakes;
			turn.message = "Tu lances une " + name + " !";
			m_stage.sound(BattleSound::BallThrow);
		} else {
			turn.message = "Tu utilises " + name + " !";
			m_stage.sound(BattleSound::Cast);
		}
		m_stage.cue(BattleCue{ turn.ball ? "ball" : "potion", name });
	}
	auto duration = BattleTurn::ActionDuration;
	if (turn.ball) {
		const auto end = turn.ballEnd();
		duration = end + (turn.caught ? 1700U : 1100U);
		if (crossed(BattleTurn::BallFlight)) {
			m_stage.sound(BattleSound::BallIn);
		}
		if (crossed(BattleTurn::BallDrop - 120)) {
			m_stage.sound(BattleSound::BallDrop);
		}
		for (auto k = 1; k <= turn.shakes; k++) {
			if (crossed(BattleTurn::BallDrop + static_cast<unsigned int>(k - 1) * BattleTurn::ShakeTime + 200)) {
				m_stage.sound(BattleSound::BallShake);
			}
		}
		if (crossed(end)) {
			turn.hit = true;
			turn.hitAge = 0;
			turn.messageAge = 0;
			const auto name = turn.opponentName;
			if (turn.caught && standing(action.target)) {
				CaptureFighter(m_entities, m_ged, m_worldState, m_stage, action.target);
				turn.message = "Gotcha ! " + name + " est capturé !";
				m_stage.sound(BattleSound::BallCaught);
				m_stage.sound(BattleSound::ClashShine);
			} else {
				turn.message = "Oh non ! " + name + " s'est libéré !";
				m_stage.sound(BattleSound::BallOpen);
			}
		}
	} else if (!turn.hit && turn.time >= BattleTurn::HitDelay) {
		turn.hit = true;
		turn.hitAge = 0;
		const auto data = Bag::data(action.item);
		if (data.has_value() && data->heal > 0 && standing(action.attacker)) {
			turn.healed = HealFighter(m_entities, m_stage, m_worldState.getBag(), action.item, data->heal, action.attacker);
		}
		if (turn.healed <= 0) {
			turn.message = "Ça n'a aucun effet...";
			turn.messageAge = 0;
		}
	}
	if (turn.time >= duration) {
		const auto captured = turn.ball && turn.caught;
		m_action++;
		m_announced = false;
		turn.time = 0;
		turn.hit = false;
		turn.attacker = 0;
		turn.item = -1;
		turn.ball = false;
		turn.message.clear();
		// Captured : the clash is over, the battle is won
		if (captured) {
			step(BattleTurn::Step::Return);
		}
	}
}
void ClashSystem::hit(const Action& action) {
	auto& turn = m_stage.turn;
	turn.hit = true;
	turn.hitAge = 0;
	turn.damage = 0;
	if (!standing(action.attacker) || !m_entities.hasComponent<SkillsHolderComponent>(action.attacker)) {
		return;
	}
	// The whole skill at once (no particles to share its power), it cannot miss
	auto skill = m_entities.getComponent<SkillsHolderComponent>(action.attacker).skills[static_cast<std::size_t>(action.skill)];
	skill.particleNumber = 1;
	m_stage.sound(BattleSound::ClashHit);
	// What it did, said after the hit as in the first games
	auto effects = std::vector<SkillEffect>{};
	auto effectiveness = 100;
	const auto damaging = skill.power > 0 || skill.fixedDamage > 0;
	if (skill.kind == SkillKind::Self) {
		ApplySkillEffects(m_entities, m_stage, m_rules, skill, action.attacker, 0, &effects);
	} else if (standing(action.target)) {
		auto& hp = m_componentAccessor.get<BattleComponent>(action.target).hp;
		const auto before = hp;
		auto& target = m_componentAccessor.get<FighterComponent>(action.target);
		target.invulnerable = 0;
		const auto guarded = target.guard > 0;
		if (damaging) {
			auto battleHit = BattleHit{};
			battleHit.attacker = action.attacker;
			battleHit.target = action.target;
			battleHit.skill = &skill;
			const auto toTarget = center(action.target) - center(action.attacker);
			const auto length = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
			battleHit.direction = length < 0.001F ? ska::Point<float>{ 1.F, 0.F } : ska::Point<float>{ toTarget.x / length, toTarget.y / length };
			battleHit.impact = center(action.target);
			battleHit.effects = &effects;
			battleHit.effectiveness = &effectiveness;
			ApplyBattleHit(m_entities, m_ged, m_rules, m_stage, battleHit);
		} else if (guarded) {
			MarkText(m_entities, m_stage, action.target, "BLOQUÉ", 1);
			m_stage.sound(BattleSound::Guard);
		} else {
			ApplySkillEffects(m_entities, m_stage, m_rules, skill, action.attacker, action.target, &effects);
		}
		turn.damage = std::max(0, before - hp);
		if (turn.damage > 0) {
			turn.combo = action.attacker == m_stage.player ? turn.combo + 1 : 0;
		}
		if (hp <= 0) {
			turn.knockedOut = action.target;
		}
		if (guarded) {
			m_followUps.push_back(named(action.target) + " est protégé !");
		} else if (damaging && effectiveness == 0) {
			m_followUps.push_back("Ça n'affecte pas " + named(action.target) + "...");
		} else if (damaging && effectiveness > 100) {
			m_followUps.push_back("C'est super efficace !");
		} else if (damaging && effectiveness < 100) {
			m_followUps.push_back("Ce n'est pas très efficace...");
		}
	}
	for (const auto& effect : effects) {
		if (effect.kind == SkillEffect::Kind::Flinch) {
			// It loses its action of this round (when it has not played yet)
			m_flinched = effect.fighter;
			continue;
		}
		const auto message = SkillEffectMessage(effect, named(effect.fighter));
		if (!message.empty()) {
			m_followUps.push_back(message);
		}
	}
	// A skill that only changes things and changed nothing
	if (!damaging && effects.empty() && m_followUps.empty()) {
		m_followUps.push_back("Mais cela échoue !");
	}
}

std::string ClashSystem::named(ska::EntityId entity) const {
	const auto wild = m_componentAccessor.get<FighterComponent>(entity).team != 0;
	return who(entity) + (wild ? " sauvage" : "");
}

std::string ClashSystem::blocked(ska::EntityId attacker) {
	auto& fighter = m_componentAccessor.get<FighterComponent>(attacker);
	if (fighter.status == FighterStatus::Sleep) {
		return named(attacker) + " dort profondément...";
	}
	if (m_flinched == attacker) {
		return named(attacker) + " a peur ! Il ne peut pas attaquer !";
	}
	// A status makes it fail sometimes : 1 time in "failOdds" (Data/Battle/status.miniska), as in the real time battle
	const auto failOdds = fighter.status == FighterStatus::None ? 0 : m_rules.statusFailOdds(StatusKey(fighter.status));
	if (failOdds > 0 && ska::NumberUtils::random(0, failOdds - 1) == 0) {
		return named(attacker) + " est paralysé ! Il ne peut pas attaquer !";
	}
	return "";
}

void ClashSystem::tick(const Action& action) {
	auto& turn = m_stage.turn;
	turn.hit = true;
	turn.hitAge = 0;
	turn.damage = 0;
	if (!standing(action.attacker)) {
		return;
	}
	auto& fighter = m_componentAccessor.get<FighterComponent>(action.attacker);
	auto& battle = m_componentAccessor.get<BattleComponent>(action.attacker);
	const auto damage = std::min(battle.hp, fighter.statusDamage);
	battle.hp -= damage;
	fighter.hpDelay = 300;
	fighter.statusBurst = FighterComponent::StatusBurstDuration;
	turn.damage = damage;
	m_stage.sound(BattleSound::Status);
	// Leech : the hit points heal the one who gave it
	if (fighter.statusLeech && standing(fighter.statusSource)) {
		auto& source = m_componentAccessor.get<BattleComponent>(fighter.statusSource);
		source.hp = std::min(source.maxHp, source.hp + damage);
	}
	if (battle.hp <= 0) {
		turn.knockedOut = action.attacker;
	}
	SLOG(ska::LogLevel::Info) << "Battle : the clash, round " << turn.round << ", its status hurts " << who(action.attacker) << " (" << damage << ")";
}

void ClashSystem::act(unsigned int before) {
	auto& turn = m_stage.turn;
	if (m_action >= m_actions.size()) {
		endRound();
		return;
	}
	const auto action = m_actions[m_action];
	if (action.item >= 0) {
		actItem(action, before);
		return;
	}
	if (!m_announced) {
		// Its message, a K.O. skips it
		if (!standing(action.attacker)) {
			m_action++;
			turn.time = 0;
			return;
		}
		m_announced = true;
		m_blocked = false;
		m_followUps.clear();
		m_extra = 0;
		turn.attacker = 0;
		turn.target = 0;
		turn.skillSprite = -1;
		turn.contact = false;
		turn.hit = false;
		turn.tick = false;
		turn.damage = 0;
		turn.messageAge = 0;
		if (!action.notice.empty()) {
			// A message only (a status wears off)
			turn.message = action.notice;
			m_blocked = true;
		} else if (action.tick) {
			const auto status = m_componentAccessor.get<FighterComponent>(action.attacker).status;
			const auto name = named(action.attacker);
			turn.message = status == FighterStatus::Leech ? "Vampigraine draine l'énergie de " + name + " !" :
				(status == FighterStatus::Burn ? name + " souffre de sa brûlure !" : name + " souffre du poison !");
			turn.target = action.attacker;
			turn.tick = true;
		} else {
			const auto stopped = blocked(action.attacker);
			if (!stopped.empty()) {
				turn.message = stopped;
				m_blocked = true;
				m_componentAccessor.get<FighterComponent>(action.attacker).statusBurst = FighterComponent::StatusBurstDuration;
				m_stage.sound(BattleSound::Status);
			} else {
				const auto& skill = m_entities.getComponent<SkillsHolderComponent>(action.attacker).skills[static_cast<std::size_t>(action.skill)];
				turn.message = named(action.attacker) + " utilise " + skill.name + " !";
				turn.attacker = action.attacker;
				turn.target = action.target;
				turn.skillSprite = skill.id;
				turn.contact = skill.contact;
				m_stage.sound(BattleSound::ClashAttack);
				m_stage.sound(BattleSound::Cast);
			}
		}
	}
	const auto next = [&]() {
		m_action++;
		m_announced = false;
		turn.time = 0;
		turn.hit = false;
		turn.tick = false;
		turn.attacker = 0;
		turn.target = 0;
		turn.message.clear();
	};
	if (m_blocked) {
		if (turn.time >= NoticeDuration) {
			next();
		}
		return;
	}
	if (action.tick) {
		if (!turn.hit && turn.time >= BattleTurn::HitDelay / 2) {
			tick(action);
		}
	} else {
		// The wind up, then it charges
		if (before < 420 && turn.time >= 420) {
			m_stage.sound(BattleSound::Dash);
		}
		if (!turn.hit && turn.time >= BattleTurn::HitDelay) {
			hit(action);
		}
	}
	if (turn.time >= BattleTurn::ActionDuration + m_extra) {
		// Its effectiveness, its effects : a message each
		if (!m_followUps.empty()) {
			turn.message = m_followUps.front();
			turn.messageAge = 0;
			m_followUps.erase(m_followUps.begin());
			m_extra += FollowUpDuration;
			return;
		}
		if (turn.knockedOut != 0) {
			step(BattleTurn::Step::KnockOut);
			return;
		}
		next();
	}
}

void ClashSystem::endRound() {
	// The statuses that hurt : an action each (poison, burn, leech)
	if (m_roundEnd == 0) {
		m_roundEnd = 1;
		for (const auto entity : { m_stage.player, m_opponent }) {
			if (!standing(entity)) {
				continue;
			}
			const auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
			if (fighter.status != FighterStatus::None && fighter.statusDamage > 0) {
				auto action = Action{ entity, fighter.statusSource, -1, -1 };
				action.tick = true;
				m_actions.push_back(action);
			}
		}
		if (m_action < m_actions.size()) {
			return;
		}
	}
	// The time of the round passes : what wears off (its message)
	if (m_roundEnd == 1) {
		m_roundEnd = 2;
		passRound();
		if (m_action < m_actions.size()) {
			return;
		}
	}
	step(BattleTurn::Step::RoundEnd);
}

void ClashSystem::passRound() {
	const auto time = m_rules.clashRoundTime();
	const auto wear = [time](unsigned int& left) {
		const auto before = left;
		left = left > time ? left - time : 0;
		return before > 0 && left == 0;
	};
	for (const auto entity : { m_stage.player, m_opponent }) {
		if (!standing(entity)) {
			continue;
		}
		auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
		const auto name = named(entity);
		auto notice = [this, entity](std::string text) {
			auto action = Action{ entity, 0, -1, -1 };
			action.notice = std::move(text);
			m_actions.push_back(action);
		};
		// Protected for its round only
		fighter.guard = 0;
		if (fighter.status != FighterStatus::None && wear(fighter.statusTime)) {
			const auto status = fighter.status;
			fighter.clearStatus();
			switch (status) {
			case FighterStatus::Sleep: notice(name + " se réveille !"); break;
			case FighterStatus::Poison: notice(name + " n'est plus empoisonné."); break;
			case FighterStatus::Burn: notice("La brûlure de " + name + " est guérie."); break;
			case FighterStatus::Paralysis: notice(name + " n'est plus paralysé."); break;
			case FighterStatus::Leech: notice("Vampigraine se dissipe."); break;
			default: break;
			}
		}
		auto reset = false;
		for (std::size_t stat = 0; stat < fighter.stages.size(); stat++) {
			if (wear(fighter.stageTime[stat])) {
				fighter.stages[stat] = 0;
				fighter.stageMultipliers[stat] = 1.F;
				reset = true;
			}
		}
		if (reset) {
			notice("Les statistiques de " + name + " reviennent à la normale.");
		}
	}
}

void ClashSystem::end() {
	m_stage.turn = BattleTurn{};
	m_lastTurn = m_time;
	m_hadTurn = true;
	m_actions.clear();
	music(false);
	interface(false);
	SLOG(ska::LogLevel::Info) << "Battle : the clash is over, the real time goes on";
}

void ClashSystem::interface(bool hidden) {
	auto event = InterfaceEvent{};
	event.hidden = hidden;
	m_ged.ska::Observable<InterfaceEvent>::notifyObservers(event);
}

void ClashSystem::music(bool clash) {
	if (clash == m_musicPlayed) {
		return;
	}
	auto& played = clash ? m_music : m_worldState.getWorldMusic();
	auto event = ska::SoundEvent{ &played, ska::SoundEventType::PLAY_MUSIC };
	m_ged.ska::Observable<ska::SoundEvent>::notifyObservers(event);
	m_musicPlayed = clash;
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
	// The battle ended meanwhile (it should not : the K.O. waits for the end of the clash)
	if (m_stage.phase() != BattlePhase::Fight) {
		end();
		return;
	}
	const auto before = turn.time;
	turn.time += ellapsedTime;
	turn.total += ellapsedTime;
	turn.messageAge += ellapsedTime;
	turn.hitAge += ellapsedTime;
	// The battle stays stopped during the clash (the bars drain : real time)
	m_stage.hitStop(120);
	intro(before, turn.time);
	const auto ai = standing(m_stage.player) && m_componentAccessor.get<FighterComponent>(m_stage.player).controlledByAI;
	switch (turn.step) {
	case BattleTurn::Step::Impact:
		if (turn.time >= BattleTurn::ImpactDuration) {
			step(BattleTurn::Step::Zoom);
		}
		break;
	case BattleTurn::Step::Zoom:
		if (turn.time >= BattleTurn::ZoomDuration) {
			step(BattleTurn::Step::Shatter);
		}
		break;
	case BattleTurn::Step::Shatter:
		if (turn.time >= BattleTurn::ShatterDuration) {
			step(BattleTurn::Step::Versus);
		}
		break;
	case BattleTurn::Step::Versus:
		if (turn.time >= BattleTurn::VersusDuration) {
			step(BattleTurn::Step::Rebuild);
		}
		break;
	case BattleTurn::Step::Rebuild:
		if (turn.time >= BattleTurn::RebuildDuration) {
			step(BattleTurn::Step::Round);
		}
		break;
	case BattleTurn::Step::Round:
		if (turn.time >= BattleTurn::RoundDuration) {
			step(BattleTurn::Step::Menu);
		}
		break;
	case BattleTurn::Step::Menu:
		if (ai && turn.time >= 900) {
			choose(static_cast<std::size_t>(strongest(m_stage.player)));
		} else if (turn.time >= BattleTurn::MenuDuration) {
			// The time is up : the skill chosen (in the bag : the first skill)
			turn.bag = false;
			choose(turn.selected < turn.skills.size() ? turn.selected : 0);
		}
		break;
	case BattleTurn::Step::Acting:
		act(before);
		break;
	case BattleTurn::Step::RoundEnd:
		if (turn.time >= BattleTurn::RoundEndDuration) {
			if (turn.round < turn.rounds && standing(m_stage.player) && standing(m_opponent)) {
				turn.round++;
				m_choice = Ui::ChoiceList{};
				m_choice.setCount(turn.skills.size() + 1);
				turn.bag = false;
				step(BattleTurn::Step::Round);
			} else {
				step(BattleTurn::Step::Return);
			}
		}
		break;
	case BattleTurn::Step::KnockOut:
		if (turn.time >= BattleTurn::KnockOutDuration) {
			step(BattleTurn::Step::Return);
		}
		break;
	case BattleTurn::Step::Return:
		if (turn.time >= BattleTurn::ReturnDuration) {
			end();
		}
		break;
	}
}
