#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleHit.h"
#include <algorithm>
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "BattleComponent.h"
#include "BattleRules.h"
#include "BattleStage.h"
#include "FighterComponent.h"
#include "BattleEffects.h"
#include "SkillDescriptor.h"
#include "StatisticsChangeEvent.h"
#include "System/FighterSystem.h"

int ApplyBattleHit(ska::EntityManager& entities, PokemonGameEventDispatcher& ged, BattleRules& rules, BattleStage& stage, const BattleHit& hit) {
	if (hit.skill == nullptr || !entities.hasComponent<BattleComponent>(hit.target) || !entities.hasComponent<FighterComponent>(hit.target)) {
		return 0;
	}
	const auto& skill = *hit.skill;
	auto& fighter = entities.getComponent<FighterComponent>(hit.target);
	// Dodging (the start of its dash) : nothing touches it
	if (fighter.invulnerable > 0) {
		MarkText(entities, stage, hit.target, "ESQUIVE", 1);
		return -1;
	}
	// Protected : the hit does nothing
	if (fighter.guard > 0) {
		MarkText(entities, stage, hit.target, "BLOQU\xC9", 1);
		stage.hitStop(40);
		stage.sound(BattleSound::Guard);
		return 0;
	}

	auto& defender = entities.getComponent<BattleComponent>(hit.target);
	const auto* attacker = entities.hasComponent<BattleComponent>(hit.attacker) ? &entities.getComponent<BattleComponent>(hit.attacker) : nullptr;
	const auto* attackerFighter = entities.hasComponent<FighterComponent>(hit.attacker) ? &entities.getComponent<FighterComponent>(hit.attacker) : nullptr;
	// The statistics with their changes in the battle ; a burn halves the physical attack
	auto attack = attacker == nullptr ? 1.F : static_cast<float>(skill.special ? attacker->speAttack : attacker->attack);
	if (attackerFighter != nullptr) {
		attack *= attackerFighter->stageMultipliers[StatAttack];
		// Its status changes its attack (a burn : the physical attacks, Data/Battle/status.miniska)
		if (attackerFighter->status != FighterStatus::None) {
			const auto percent = rules.statusAttackPercent(StatusKey(attackerFighter->status), skill.special);
			if (percent != 100) {
				attack *= static_cast<float>(percent) / 100.F;
			}
		}
	}
	const auto defense = static_cast<float>(skill.special ? defender.speDefense : defender.defense) * fighter.stageMultipliers[StatDefense];
	const auto effectiveness = rules.effectiveness(skill.type, defender.type1, defender.type2);
	const auto stab = attacker != nullptr && (skill.type == attacker->type1 || skill.type == attacker->type2);
	const auto damage = skill.fixedDamage > 0 ? (effectiveness > 0 ? skill.fixedDamage : 0) :
		rules.damage(skill.power, std::max(1, static_cast<int>(attack)), std::max(1, static_cast<int>(defense)), attacker == nullptr ? 1 : attacker->level, effectiveness, stab);
	defender.hp -= damage;
	SLOG_STATIC(ska::LogLevel::Debug, BattleHit) << "Battle : " << (fighter.player ? "the pokemon of the player" : "the wild pokemon") << " is hit, "
		<< damage << " damage, " << defender.hp << " / " << defender.maxHp << " hit points left (effectiveness " << effectiveness << "%)";
	if (skill.power > 0 || skill.fixedDamage > 0) {
		if (effectiveness == 0) {
			MarkText(entities, stage, hit.target, "AUCUN EFFET", 0);
		} else if (effectiveness > 100) {
			MarkText(entities, stage, hit.target, "SUPER EFFICACE !", 2);
		} else if (effectiveness < 100) {
			MarkText(entities, stage, hit.target, "PEU EFFICACE...", 1);
		}
	}
	// A hit wakes it up (Data/Battle/status.miniska)
	if (fighter.status != FighterStatus::None && rules.statusWakes(StatusKey(fighter.status), damage)) {
		fighter.clearStatus();
	}

	// A hit during the startup of an attack cancels it (its cooldown is lost)
	const auto strong = std::min(damage, 12);
	fighter.flash = FighterSystem::FlashDuration;
	fighter.hpDelay = 500;
	fighter.windUp = 0;
	fighter.pendingSkill = -1;
	if (damage > 0) {
		stage.sound(strong >= 6 || effectiveness > 100 ? BattleSound::StrongHit : BattleSound::Hit);
		fighter.stun = static_cast<unsigned int>(260 + strong * 15);
		fighter.barShake = 320;
		const auto push = 160.F + static_cast<float>(skill.knockback) * 9.F + static_cast<float>(strong) * 12.F;
		fighter.knockback = ska::Point<float>{ hit.direction.x * push, hit.direction.y * push };
		stage.hitStop(static_cast<unsigned int>(60 + strong * 6));
		stage.shake(180 + static_cast<unsigned int>(strong) * 10, std::max(2, std::min(9, 2 + strong / 2)));
	} else {
		// A skill without damage only touches
		fighter.stun = 100;
		fighter.knockback = ska::Point<float>{ hit.direction.x * 90.F, hit.direction.y * 90.F };
		stage.hitStop(25);
	}

	auto impactMark = BattleMark{};
	impactMark.kind = BattleMark::Kind::Impact;
	impactMark.position = hit.impact;
	impactMark.value = strong;
	stage.mark(impactMark);

	const auto& targetPosition = entities.getComponent<ska::PositionComponent>(hit.target);
	const auto& targetHitbox = entities.getComponent<ska::HitboxComponent>(hit.target);
	const auto targetCenter = ska::PositionComponent::getCenterPosition(targetPosition, targetHitbox);
	auto damageMark = BattleMark{};
	damageMark.kind = BattleMark::Kind::Damage;
	damageMark.position = ska::Point<float>{ targetCenter.x, targetCenter.y - static_cast<float>(targetHitbox.height) };
	damageMark.value = damage;
	damageMark.onPlayer = fighter.player;
	stage.mark(damageMark);

	// The effects of the skill (statistics, status) when it touched
	if (effectiveness > 0) {
		ApplySkillEffects(entities, stage, rules, skill, hit.attacker, hit.target);
	}

	StatisticsChangeEvent event(hit.target, defender, hit.attacker);
	ged.ska::Observable<StatisticsChangeEvent>::notifyObservers(event);
	return damage;
}
