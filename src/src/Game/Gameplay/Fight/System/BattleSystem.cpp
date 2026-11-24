#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleSystem.h"
#include <cmath>
#include <cstdlib>
#include "../SkillComponent.h"
#include "Core/Inputs/InputContextManager.h"
#include "../../CustomEntityManager.h"
#include "Core/Utils/RectangleUtils.h"
#include "Graphic/GraphicComponent.h"
#include "../BattleRules.h"
#include "FighterSystem.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "../BattleHit.h"
#include "../BattleEffects.h"
#include "../FighterComponent.h"

BattleSystem::BattleSystem(CustomEntityManager& em, PokemonGameEventDispatcher& ged, BattleRules& rules, BattleStage& stage, unsigned int blockSize) :
	System(em),
	SubObserver<ska::InputKeyEvent>(std::bind(&BattleSystem::onKeyInputEvent, this, std::placeholders::_1), ged),
	m_customEM(em),
	m_ged(ged),
	m_rules(rules),
	m_stage(stage),
	m_blockSize(blockSize == 0 ? 1 : blockSize) {
}

ska::Point<float> BattleSystem::center(ska::EntityId entity) const {
	return ska::PositionComponent::getCenterPosition(m_componentAccessor.get<ska::PositionComponent>(entity), m_componentAccessor.get<ska::HitboxComponent>(entity));
}

bool BattleSystem::fighting(ska::EntityId fighter) const {
	const auto& processed = getEntities();
	return processed.find(fighter) != processed.end() && m_componentAccessor.get<BattleComponent>(fighter).hp > 0 && !m_componentAccessor.get<FighterComponent>(fighter).knockedOut;
}

bool BattleSystem::canAct(ska::EntityId fighter) const {
	if (m_stage.phase() != BattlePhase::Fight || m_stage.frozen() || !fighting(fighter)) {
		return false;
	}
	return !m_componentAccessor.get<FighterComponent>(fighter).busy();
}

bool BattleSystem::skillReady(unsigned int index, ska::EntityId from) const {
	if (!fighting(from) || index >= 4) {
		return false;
	}
	const auto& skill = m_componentAccessor.get<SkillsHolderComponent>(from).skills[index];
	const auto lastUse = m_componentAccessor.get<FighterComponent>(from).lastUses[index];
	return skill.particleNumber > 0 && (lastUse == 0 || m_time - lastUse >= skill.cooldown);
}

float BattleSystem::cooldown(unsigned int index, ska::EntityId from) const {
	if (!fighting(from) || index >= 4) {
		return 0.F;
	}
	const auto& skill = m_componentAccessor.get<SkillsHolderComponent>(from).skills[index];
	const auto lastUse = m_componentAccessor.get<FighterComponent>(from).lastUses[index];
	if (lastUse == 0 || skill.cooldown == 0 || m_time - lastUse >= skill.cooldown) {
		return 0.F;
	}
	return 1.F - static_cast<float>(m_time - lastUse) / static_cast<float>(skill.cooldown);
}

bool BattleSystem::useSkill(unsigned int index, ska::EntityId from) {
	if (!canAct(from) || !skillReady(index, from)) {
		return false;
	}
	auto& fighter = m_componentAccessor.get<FighterComponent>(from);
	// 0 means "never used"
	fighter.lastUses[index] = m_time == 0 ? 1 : m_time;
	// A status makes it fail a skill sometimes (the cooldown is lost) : 1 time in "failOdds" (Data/Battle/status.miniska)
	const auto failOdds = fighter.status == FighterStatus::None ? 0 : m_rules.statusFailOdds(StatusKey(fighter.status));
	if (failOdds > 0 && ska::NumberUtils::random(0, failOdds - 1) == 0) {
		fighter.stun = std::max(fighter.stun, m_rules.statusStun(StatusKey(fighter.status)));
		MarkText(m_customEM, m_stage, from, "PARALYS\xC9", 2);
		return true;
	}
	const auto& skill = m_componentAccessor.get<SkillsHolderComponent>(from).skills[index];
	// The startup : it stands still, facing its opponent (it is committed to that direction)
	fighter.windUp = std::max(1u, skill.startup);
	fighter.windUpDuration = fighter.windUp;
	fighter.recoveryDuration = skill.recovery;
	fighter.pendingSkill = static_cast<int>(index);
	fighter.contactAttack = skill.contact;
	m_stage.sound(BattleSound::Cast);
	const auto position = center(from);
	const auto toOpponent = center(fighter.opponent) - position;
	const auto length = std::sqrt(toOpponent.x * toOpponent.x + toOpponent.y * toOpponent.y);
	fighter.attackDirection = length < 0.001F ? ska::Point<float>{ 1.F, 0.F } : ska::Point<float>{ toOpponent.x / length, toOpponent.y / length };
	SLOG(ska::LogLevel::Debug) << "Battle t=" << m_time << " : " << (fighter.player ? "the pokemon of the player" : "the wild pokemon") << " uses its skill " << index
		<< " (" << m_componentAccessor.get<SkillsHolderComponent>(from).skills[index].name << ") from " << position.x << ", " << position.y;
	return true;
}

ska::EntityId BattleSystem::playedByKeys() const {
	for (const auto entity : getEntities()) {
		const auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
		if (fighter.player && !fighter.controlledByAI) {
			return entity;
		}
	}
	return 0;
}

bool BattleSystem::onKeyInputEvent(ska::InputKeyEvent& event) {
	const auto player = playedByKeys();
	if (player == 0) {
		return true;
	}
	const auto& actions = event.icm.getActions();
	const ska::InputAction skillActions[] = { ska::InputAction::ShotSkill1, ska::InputAction::ShotSkill2, ska::InputAction::ShotSkill3, ska::InputAction::ShotSkill4 };
	for (auto index = 0u; index < 4; index++) {
		if (actions[skillActions[index]] && !useSkill(index, player) && m_stage.phase() == BattlePhase::Fight) {
			m_bufferedSkill = static_cast<int>(index);
			m_bufferedUntil = m_time + InputBuffer;
		}
	}
	return true;
}

void BattleSystem::decide(ska::EntityId entity, FighterComponent& fighter) {
	if (!canAct(entity) || !fighting(fighter.opponent)) {
		return;
	}
	const auto& bc = m_componentAccessor.get<BattleComponent>(entity);
	const auto toOpponent = center(fighter.opponent) - center(entity);
	const auto pixels = std::sqrt(toOpponent.x * toOpponent.x + toOpponent.y * toOpponent.y);
	const auto distance = static_cast<int>(pixels / m_blockSize);

	// The skills it can use now : ready, and reaching its opponent. "reach" : the longest of the ready skills
	const auto& skills = m_componentAccessor.get<SkillsHolderComponent>(entity).skills;
	auto ready = std::vector<bool>{};
	auto reach = 0;
	for (auto index = 0u; index < 4; index++) {
		const auto skillReadyNow = skillReady(index, entity);
		if (skillReadyNow && skills[index].kind != SkillKind::Self) {
			reach = std::max(reach, skills[index].range);
		}
		ready.push_back(skillReadyNow && (skills[index].kind == SkillKind::Self || pixels <= static_cast<double>(skills[index].range)));
	}
	const auto decision = m_rules.decide(bc.monster, distance, bc.hp, bc.maxHp, ready);
	const auto intent = m_rules.move(bc.monster, static_cast<int>(pixels), reach, bc.hp, bc.maxHp);
	fighter.intent = static_cast<FighterIntent>(intent);
	SLOG(ska::LogLevel::Debug) << "Battle t=" << m_time << " : " << (fighter.player ? "player" : "wild") << " at " << static_cast<int>(center(entity).x) << ", "
		<< static_cast<int>(center(entity).y) << " distance " << static_cast<int>(pixels) << " px, decision " << decision << ", movement " << intent;
	if (decision >= 0) {
		useSkill(static_cast<unsigned int>(decision), entity);
	}
}

void BattleSystem::retarget() {
	// Each fighter fights the nearest fighter of the other side still standing (it keeps its opponent when there is none)
	for (const auto entity : getEntities()) {
		auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
		if (!fighting(entity)) {
			continue;
		}
		const auto position = center(entity);
		auto best = -1.F;
		for (const auto other : getEntities()) {
			if (!fighting(other) || m_componentAccessor.get<FighterComponent>(other).team == fighter.team) {
				continue;
			}
			const auto toOther = center(other) - position;
			const auto distance = toOther.x * toOther.x + toOther.y * toOther.y;
			if (best < 0.F || distance < best) {
				best = distance;
				fighter.opponent = other;
			}
		}
	}
}

void BattleSystem::refresh(unsigned int) {
	const auto delta = m_stage.delta();
	m_time += delta;
	retarget();

	for (const auto entity : getEntities()) {
		updateAttack(entity, m_componentAccessor.get<FighterComponent>(entity), delta);
	}
	if (m_bufferedSkill >= 0) {
		const auto player = playedByKeys();
		if (player == 0 || m_time > m_bufferedUntil || useSkill(static_cast<unsigned int>(m_bufferedSkill), player)) {
			m_bufferedSkill = -1;
		}
	}

	if (m_stage.phase() == BattlePhase::Fight) {
		for (const auto entity : getEntities()) {
			auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
			if (fighter.controlledByAI && fighting(entity)) {
				considerDodging(entity, fighter);
			}
			if (fighter.controlledByAI && m_time >= fighter.nextDecision) {
				// A little irregular : the AI do not act at the same time
				fighter.nextDecision = m_time + DecisionDelay + static_cast<unsigned long>(ska::NumberUtils::random(0, 120));
				decide(entity, fighter);
			}
		}
	}
}

void BattleSystem::createSkill(const unsigned int index, ska::EntityId from) {
	auto& shc = m_componentAccessor.get<SkillsHolderComponent>(from);
	if (index >= shc.skills.size()) {
		return;
	}
	const auto target = m_componentAccessor.get<FighterComponent>(from).opponent;

	// From the side of the fighter facing its target, towards the target (where it is now)
	const auto origin = center(from);
	const auto aim = center(target);
	const auto toTarget = aim - origin;
	const auto length = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
	const auto direction = length < 0.001F ? ska::Point<float>{ 1.F, 0.F } : ska::Point<float>{ toTarget.x / length, toTarget.y / length };
	const auto& hc = m_componentAccessor.get<ska::HitboxComponent>(from);
	const auto front = static_cast<float>(std::max(hc.width, hc.height)) / 2.F;
	const auto start = ska::Point<float>{ origin.x + direction.x * front, origin.y + direction.y * front };

	const auto particles = shc.skills[index].particleNumber;
	for (auto i = 0; i < particles; i++) {
		// Each particle is the skill used (not the skill at the index of the particle)
		auto skill = m_customEM.createSkill(shc, index);

		const auto& scPtr = m_componentPossibleAccessor.get<SkillComponent>(skill);
		if (scPtr == nullptr) {
			continue;
		}

		auto& gcSkill = m_componentAccessor.get<ska::GraphicComponent>(skill);
		auto& sc = *scPtr;
		const auto half = ska::Point<int>(gcSkill.animatedSprites[0].getWidth() / 2, gcSkill.animatedSprites[0].getHeight() / 2);
		sc.origin = ska::Point<int>{ static_cast<int>(start.x), static_cast<int>(start.y) } - half;
		m_componentAccessor.add<ska::PositionComponent>(skill, sc.origin);
		sc.target = ska::Point<int>{ static_cast<int>(aim.x), static_cast<int>(aim.y) } - half;
		sc.battler = from;
	}
}

void BattleSystem::updateAttack(ska::EntityId entity, FighterComponent& fighter, unsigned int delta) {
	if (delta == 0) {
		return;
	}
	// Recovery, then free
	if (fighter.recovery > 0) {
		fighter.recovery = fighter.recovery > delta ? fighter.recovery - delta : 0;
		return;
	}
	// Lunge of a contact skill, then its recovery
	if (fighter.lunge > 0) {
		fighter.lunge = fighter.lunge > delta ? fighter.lunge - delta : 0;
		if (fighter.lunge == 0) {
			fighter.recovery = fighter.recoveryDuration;
		}
		return;
	}
	if (fighter.windUp == 0) {
		return;
	}
	// The startup ends : the skill goes
	fighter.windUp = fighter.windUp > delta ? fighter.windUp - delta : 0;
	if (fighter.windUp > 0 || fighter.pendingSkill < 0) {
		return;
	}
	const auto index = static_cast<unsigned int>(fighter.pendingSkill);
	fighter.pendingSkill = -1;
	if (m_stage.phase() != BattlePhase::Fight || !fighting(entity) || !fighting(fighter.opponent)) {
		return;
	}
	const auto& skill = m_componentAccessor.get<SkillsHolderComponent>(entity).skills[index];
	if (skill.kind == SkillKind::Area || skill.kind == SkillKind::Self) {
		area(index, entity, fighter);
		fighter.release = FighterSystem::ReleaseDuration;
		fighter.recovery = fighter.recoveryDuration;
	} else if (fighter.contactAttack) {
		fighter.lunge = LungeDuration;
		contact(index, entity, fighter);
	} else {
		createSkill(index, entity);
		fighter.release = FighterSystem::ReleaseDuration;
		fighter.recovery = fighter.recoveryDuration;
	}
}

void BattleSystem::contact(unsigned int index, ska::EntityId entity, FighterComponent& fighter) {
	// It hits its opponent when it is in its range, in front of it (the direction chosen at the startup)
	const auto& skill = m_componentAccessor.get<SkillsHolderComponent>(entity).skills[index];
	const auto origin = center(entity);
	const auto toOpponent = center(fighter.opponent) - origin;
	const auto distance = std::sqrt(toOpponent.x * toOpponent.x + toOpponent.y * toOpponent.y);
	const auto facing = distance < 0.001F ? 1.F : (toOpponent.x * fighter.attackDirection.x + toOpponent.y * fighter.attackDirection.y) / distance;
	// In front : less than 70 degrees from its direction (always when they touch each other). The nearest of the
	// fighters of the other side it touches
	auto target = ska::EntityId{ 0 };
	auto nearest = -1.F;
	for (const auto other : getEntities()) {
		if (!fighting(other) || m_componentAccessor.get<FighterComponent>(other).team == fighter.team) {
			continue;
		}
		const auto toOther = center(other) - origin;
		const auto otherDistance = std::sqrt(toOther.x * toOther.x + toOther.y * toOther.y);
		const auto otherFacing = otherDistance < 0.001F ? 1.F : (toOther.x * fighter.attackDirection.x + toOther.y * fighter.attackDirection.y) / otherDistance;
		if (otherDistance <= static_cast<float>(skill.range) && (otherFacing >= 0.34F || otherDistance < 40.F) && (nearest < 0.F || otherDistance < nearest)) {
			nearest = otherDistance;
			target = other;
		}
	}
	SLOG(ska::LogLevel::Debug) << "Battle t=" << m_time << " : " << (fighter.player ? "the pokemon of the player" : "the wild pokemon") << " lunges (" << skill.name << "), "
		<< static_cast<int>(distance) << " px from its opponent : " << (target != 0 ? "it hits" : "missed");
	if (target == 0) {
		return;
	}
	auto hit = BattleHit{};
	hit.attacker = entity;
	hit.target = target;
	hit.skill = &skill;
	hit.direction = fighter.attackDirection;
	const auto& targetHitbox = m_componentAccessor.get<ska::HitboxComponent>(target);
	const auto reach = static_cast<float>(std::max(targetHitbox.width, targetHitbox.height)) / 2.F;
	const auto targetCenter = center(target);
	hit.impact = ska::Point<float>{ targetCenter.x - fighter.attackDirection.x * reach, targetCenter.y - fighter.attackDirection.y * reach };
	ApplyBattleHit(m_customEM, m_ged, m_rules, m_stage, hit);
	if (skill.showSprite) {
		auto slash = BattleMark{};
		slash.kind = BattleMark::Kind::Slash;
		slash.position = hit.impact;
		slash.value = skill.id;
		m_stage.mark(slash);
	}
}

void BattleSystem::area(unsigned int index, ska::EntityId entity, FighterComponent& fighter) {
	const auto& skill = m_componentAccessor.get<SkillsHolderComponent>(entity).skills[index];
	const auto origin = center(entity);
	auto wave = BattleMark{};
	wave.kind = BattleMark::Kind::Wave;
	wave.position = origin;
	wave.onPlayer = fighter.team == 0;
	if (skill.kind == SkillKind::Self) {
		// On itself only
		wave.value = 40;
		m_stage.mark(wave);
		ApplySkillEffects(m_customEM, m_stage, m_rules, skill, entity, 0);
		SLOG(ska::LogLevel::Debug) << "Battle t=" << m_time << " : " << (fighter.player ? "the pokemon of the player" : "a wild pokemon") << " uses " << skill.name << " on itself";
		return;
	}
	// Every fighter of the other side in its range, around it
	wave.value = skill.range;
	m_stage.mark(wave);
	auto touched = 0;
	for (const auto other : getEntities()) {
		if (!fighting(other) || m_componentAccessor.get<FighterComponent>(other).team == fighter.team) {
			continue;
		}
		const auto toOther = center(other) - origin;
		const auto distance = std::sqrt(toOther.x * toOther.x + toOther.y * toOther.y);
		if (distance > static_cast<float>(skill.range)) {
			continue;
		}
		touched++;
		if (skill.power > 0 || skill.fixedDamage > 0) {
			auto hit = BattleHit{};
			hit.attacker = entity;
			hit.target = other;
			hit.skill = &skill;
			hit.direction = distance < 0.001F ? ska::Point<float>{ 1.F, 0.F } : ska::Point<float>{ toOther.x / distance, toOther.y / distance };
			hit.impact = center(other);
			ApplyBattleHit(m_customEM, m_ged, m_rules, m_stage, hit);
		} else {
			auto& target = m_componentAccessor.get<FighterComponent>(other);
			if (target.invulnerable > 0) {
				MarkText(m_customEM, m_stage, other, "ESQUIVE", 1);
			} else if (target.guard > 0) {
				MarkText(m_customEM, m_stage, other, "BLOQU\xC9", 1);
			} else {
				ApplySkillEffects(m_customEM, m_stage, m_rules, skill, entity, other);
			}
		}
	}
	// The effects on itself, whatever it touched
	if (skill.selfStages != std::array<int, 3>{} && (skill.power > 0 || skill.targetStages != std::array<int, 3>{} || !skill.status.empty())) {
		auto selfOnly = skill;
		selfOnly.targetStages = {};
		selfOnly.status.clear();
		ApplySkillEffects(m_customEM, m_stage, m_rules, selfOnly, entity, 0);
	}
	SLOG(ska::LogLevel::Debug) << "Battle t=" << m_time << " : " << (fighter.player ? "the pokemon of the player" : "a wild pokemon") << " uses " << skill.name << " around it : " << touched << " touched";
}

void BattleSystem::considerDodging(ska::EntityId entity, FighterComponent& fighter) {
	// An attack of a fighter of the other side is coming (its startup, near enough) : the AI may dodge it, once
	for (const auto other : getEntities()) {
		if (!fighting(other)) {
			continue;
		}
		const auto& enemy = m_componentAccessor.get<FighterComponent>(other);
		if (enemy.team == fighter.team || enemy.windUp == 0 || enemy.pendingSkill < 0 || enemy.opponent != entity || fighter.dodgeChecked == other) {
			continue;
		}
		const auto& skill = m_componentAccessor.get<SkillsHolderComponent>(other).skills[static_cast<std::size_t>(enemy.pendingSkill)];
		const auto toEnemy = center(other) - center(entity);
		const auto distance = std::sqrt(toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y);
		if (skill.kind == SkillKind::Self || distance > static_cast<float>(skill.range) + 60.F) {
			continue;
		}
		fighter.dodgeChecked = other;
		const auto& bc = m_componentAccessor.get<BattleComponent>(entity);
		if (m_rules.dodge(bc.monster, static_cast<int>(enemy.windUp), bc.hp, bc.maxHp)) {
			fighter.dodgeRequested = true;
		}
	}
	// The attack is over : the next one of this enemy will be considered
	if (fighter.dodgeChecked != 0 && (!fighting(fighter.dodgeChecked) || m_componentAccessor.get<FighterComponent>(fighter.dodgeChecked).windUp == 0)) {
		fighter.dodgeChecked = 0;
	}
}
