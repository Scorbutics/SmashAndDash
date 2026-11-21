#include "Game/__internalConfig/LoggerConfig.h"
#include "SkillHitSystem.h"
#include "../BattleRules.h"

SkillHitSystem::SkillHitSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, BattleRules& rules, ska::EntityId pokemon, ska::EntityId opponent) :
	System(em),
	m_entities(em),
	m_ged(ged),
	m_rules(rules),
	m_pokemon(pokemon),
	m_opponent(opponent) {
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

void SkillHitSystem::hit(SkillComponent& skill, ska::EntityId target) {
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
	SLOG(ska::LogLevel::Debug) << "Battle : " << (target == m_pokemon ? "the pokemon of the player" : "the wild pokemon") << " is hit, "
		<< damage << " damage, " << defender->hp << " / " << defender->maxHp << " hit points left";
	StatisticsChangeEvent event(target, *defender, skill.battler);
	m_ged.ska::Observable<StatisticsChangeEvent>::notifyObservers(event);
}

void SkillHitSystem::refresh(unsigned int) {
	for (const auto skillEntity : getEntities()) {
		auto& skill = m_componentAccessor.get<SkillComponent>(skillEntity);
		if (skill.spent) {
			continue;
		}
		// A skill only hits the opponent of the Pokemon that used it
		const auto target = skill.battler == m_pokemon ? m_opponent : m_pokemon;
		if (touches(skillEntity, target)) {
			hit(skill, target);
		}
	}
}
