#include "Game/__internalConfig/LoggerConfig.h"
#include "SkillHitSystem.h"
#include <algorithm>
#include <cmath>
#include "../BattleRules.h"
#include "../BattleHit.h"
#include "../FighterComponent.h"
#include "../SkillsHolderComponent.h"

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
	if (!m_entities.hasComponent<SkillsHolderComponent>(skill.battler)) {
		return;
	}
	const auto& skills = m_entities.getComponent<SkillsHolderComponent>(skill.battler).skills;

	// Where it touched, in which direction it pushes
	const auto& skillPosition = m_componentAccessor.get<ska::PositionComponent>(skillEntity);
	const auto& skillHitbox = m_componentAccessor.get<ska::HitboxComponent>(skillEntity);
	auto hit = BattleHit{};
	hit.attacker = skill.battler;
	hit.target = target;
	hit.skill = &skills[std::min<std::size_t>(skill.skillIndex, skills.size() - 1)];
	hit.impact = ska::Point<float>{ static_cast<float>(skillPosition.x + skillHitbox.xOffset + static_cast<int>(skillHitbox.width) / 2),
		static_cast<float>(skillPosition.y + skillHitbox.yOffset + static_cast<int>(skillHitbox.height) / 2) };
	const auto direction = ska::Point<float>{ static_cast<float>(skill.target.x - skill.origin.x), static_cast<float>(skill.target.y - skill.origin.y) };
	const auto length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	hit.direction = length < 0.001F ? ska::Point<float>{} : ska::Point<float>{ direction.x / length, direction.y / length };
	if (ApplyBattleHit(m_entities, m_ged, m_rules, m_stage, hit) < 0) {
		// Dodged : it goes on (through this fighter)
		skill.dodgedBy = target;
		return;
	}
	// The skill disappears after its hit (its range becomes 0 : removed by SkillRefreshSystem)
	skill.spent = true;
	skill.range = 0;
}

void SkillHitSystem::refresh(unsigned int) {
	if (m_stage.phase() != BattlePhase::Fight || m_stage.frozen()) {
		return;
	}
	for (const auto skillEntity : getEntities()) {
		auto& skill = m_componentAccessor.get<SkillComponent>(skillEntity);
		if (skill.spent || !m_entities.hasComponent<FighterComponent>(skill.battler)) {
			continue;
		}
		// A skill hits the fighters of the other side of the Pokemon that used it (still standing)
		const auto team = m_entities.getComponent<FighterComponent>(skill.battler).team;
		for (const auto target : m_stage.fighters) {
			if (!m_entities.hasComponent<FighterComponent>(target) || !m_entities.hasComponent<BattleComponent>(target)) {
				continue;
			}
			const auto& fighter = m_entities.getComponent<FighterComponent>(target);
			if (fighter.team == team || target == skill.dodgedBy || fighter.knockedOut || fighter.inBall || m_entities.getComponent<BattleComponent>(target).hp <= 0 || !touches(skillEntity, target)) {
				continue;
			}
			hit(skillEntity, skill, target);
			if (skill.spent) {
				break;
			}
		}
		// One hit at a time : the hit stop starts
		if (m_stage.frozen()) {
			return;
		}
	}
}
