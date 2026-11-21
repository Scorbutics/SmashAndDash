#include "Game/__internalConfig/LoggerConfig.h"
#include "SkillHitSystem.h"
#include <algorithm>
#include <cmath>
#include "../BattleRules.h"
#include "FighterSystem.h"

SkillHitSystem::SkillHitSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, BattleRules& rules, BattleStage& stage) :
	System(em),
	m_entities(em),
	m_ged(ged),
	m_rules(rules),
	m_stage(stage) {
}

bool SkillHitSystem::touches(ska::EntityId skill, ska::EntityId target) {
	if (!m_entities.hasComponent<ska::PositionComponent>(target) || !m_entities.hasComponent<ska::HitboxComponent>(target)) {
		return false;
	}
	const auto* targetPosition = &m_entities.getComponent<ska::PositionComponent>(target);
	const auto* targetHitbox = &m_entities.getComponent<ska::HitboxComponent>(target);
	const auto& position = m_componentAccessor.get<ska::PositionComponent>(skill);
	const auto& hitbox = m_componentAccessor.get<ska::HitboxComponent>(skill);
	const auto left = position.x + hitbox.xOffset;
	const auto top = position.y + hitbox.yOffset;
	const auto targetLeft = targetPosition->x + targetHitbox->xOffset;
	const auto targetTop = targetPosition->y + targetHitbox->yOffset;
	return left < targetLeft + targetHitbox->width && targetLeft < left + hitbox.width &&
		top < targetTop + targetHitbox->height && targetTop < top + hitbox.height;
}

void SkillHitSystem::hit(ska::EntityId skillEntity, SkillComponent& skill, ska::EntityId target) {
	// The skill disappears after its hit (its range becomes 0 : removed by SkillRefreshSystem)
	skill.spent = true;
	skill.range = 0;
	if (!m_entities.hasComponent<BattleComponent>(target)) {
		return;
	}
	auto* defender = &m_entities.getComponent<BattleComponent>(target);
	const auto* attacker = m_entities.hasComponent<BattleComponent>(skill.battler) ? &m_entities.getComponent<BattleComponent>(skill.battler) : nullptr;
	const auto attack = attacker == nullptr ? 1 : (skill.special ? attacker->speAttack : attacker->attack);
	const auto defense = skill.special ? defender->speDefense : defender->defense;
	const auto damage = m_rules.damage(skill.power, attack, defense, attacker == nullptr ? 1 : attacker->level);
	defender->hp -= damage;
	auto& fighter = m_stage.fighter(target);
	SLOG(ska::LogLevel::Debug) << "Battle : " << (fighter.player ? "the pokemon of the player" : "the wild pokemon") << " is hit, "
		<< damage << " damage, " << defender->hp << " / " << defender->maxHp << " hit points left";

	// The feel of the hit : where it touched, in which direction it pushes
	const auto& skillPosition = m_componentAccessor.get<ska::PositionComponent>(skillEntity);
	const auto& skillHitbox = m_componentAccessor.get<ska::HitboxComponent>(skillEntity);
	const auto impact = ska::Point<float>{ static_cast<float>(skillPosition.x + skillHitbox.xOffset + static_cast<int>(skillHitbox.width) / 2),
		static_cast<float>(skillPosition.y + skillHitbox.yOffset + static_cast<int>(skillHitbox.height) / 2) };
	auto direction = ska::Point<float>{ static_cast<float>(skill.target.x - skill.origin.x), static_cast<float>(skill.target.y - skill.origin.y) };
	const auto length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	direction = length < 0.001F ? ska::Point<float>{} : ska::Point<float>{ direction.x / length, direction.y / length };

	const auto& targetPosition = m_entities.getComponent<ska::PositionComponent>(target);
	const auto& targetHitbox = m_entities.getComponent<ska::HitboxComponent>(target);
	const auto targetCenter = ska::PositionComponent::getCenterPosition(targetPosition, targetHitbox);

	const auto strong = std::min(damage, 12);
	fighter.flash = FighterSystem::FlashDuration;
	fighter.hpDelay = 500;
	fighter.windUp = 0;
	fighter.pendingSkill = -1;
	if (damage > 0) {
		fighter.stun = static_cast<unsigned int>(200 + strong * 15);
		fighter.barShake = 320;
		const auto push = 140.F + static_cast<float>(skill.knockback) * 9.F + static_cast<float>(strong) * 12.F;
		fighter.knockback = ska::Point<float>{ direction.x * push, direction.y * push };
		m_stage.hitStop(static_cast<unsigned int>(55 + strong * 6));
		m_stage.shake(180 + static_cast<unsigned int>(strong) * 10, std::max(2, std::min(9, 2 + strong / 2)));
	} else {
		// A skill without damage only touches
		fighter.stun = 80;
		fighter.knockback = ska::Point<float>{ direction.x * 80.F, direction.y * 80.F };
		m_stage.hitStop(25);
	}

	auto impactMark = BattleMark{};
	impactMark.kind = BattleMark::Kind::Impact;
	impactMark.position = impact;
	impactMark.value = strong;
	m_stage.mark(impactMark);

	auto damageMark = BattleMark{};
	damageMark.kind = BattleMark::Kind::Damage;
	damageMark.position = ska::Point<float>{ targetCenter.x, targetCenter.y - static_cast<float>(targetHitbox.height) };
	damageMark.value = damage;
	damageMark.onPlayer = fighter.player;
	m_stage.mark(damageMark);

	StatisticsChangeEvent event(target, *defender, skill.battler);
	m_ged.ska::Observable<StatisticsChangeEvent>::notifyObservers(event);
}

void SkillHitSystem::refresh(unsigned int) {
	if (m_stage.phase() != BattlePhase::Fight || m_stage.frozen()) {
		return;
	}
	for (const auto skillEntity : getEntities()) {
		auto& skill = m_componentAccessor.get<SkillComponent>(skillEntity);
		if (skill.spent || !m_stage.isFighter(skill.battler)) {
			continue;
		}
		// A skill only hits the opponent of the Pokemon that used it
		const auto target = m_stage.fighter(skill.battler).opponent;
		if (touches(skillEntity, target)) {
			hit(skillEntity, skill, target);
			// One hit at a time : the hit stop starts
			if (m_stage.frozen()) {
				return;
			}
		}
	}
}
