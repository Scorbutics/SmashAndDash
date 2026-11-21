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
#include "StatisticsChangeEvent.h"
#include "System/FighterSystem.h"

int ApplyBattleHit(ska::EntityManager& entities, PokemonGameEventDispatcher& ged, BattleRules& rules, BattleStage& stage, const BattleHit& hit) {
	if (!entities.hasComponent<BattleComponent>(hit.target) || !entities.hasComponent<FighterComponent>(hit.target)) {
		return 0;
	}
	auto& defender = entities.getComponent<BattleComponent>(hit.target);
	const auto* attacker = entities.hasComponent<BattleComponent>(hit.attacker) ? &entities.getComponent<BattleComponent>(hit.attacker) : nullptr;
	const auto attack = attacker == nullptr ? 1 : (hit.special ? attacker->speAttack : attacker->attack);
	const auto defense = hit.special ? defender.speDefense : defender.defense;
	const auto damage = rules.damage(hit.power, attack, defense, attacker == nullptr ? 1 : attacker->level);
	defender.hp -= damage;
	auto& fighter = entities.getComponent<FighterComponent>(hit.target);
	SLOG_STATIC(ska::LogLevel::Debug, BattleHit) << "Battle : " << (fighter.player ? "the pokemon of the player" : "the wild pokemon") << " is hit, "
		<< damage << " damage, " << defender.hp << " / " << defender.maxHp << " hit points left";

	// A hit during the startup of an attack cancels it (its cooldown is lost)
	const auto strong = std::min(damage, 12);
	fighter.flash = FighterSystem::FlashDuration;
	fighter.hpDelay = 500;
	fighter.windUp = 0;
	fighter.pendingSkill = -1;
	if (damage > 0) {
		fighter.stun = static_cast<unsigned int>(260 + strong * 15);
		fighter.barShake = 320;
		const auto push = 160.F + static_cast<float>(hit.knockback) * 9.F + static_cast<float>(strong) * 12.F;
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

	StatisticsChangeEvent event(hit.target, defender, hit.attacker);
	ged.ska::Observable<StatisticsChangeEvent>::notifyObservers(event);
	return damage;
}
