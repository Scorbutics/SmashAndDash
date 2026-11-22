#pragma once
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"
#include "../PokemonGameEventDispatcher.h"

namespace ska {
	class EntityManager;
}
class BattleRules;
class BattleStage;
class SkillDescriptor;

// A skill touching a fighter (a projectile, or a contact skill as Charge)
struct BattleHit {
	ska::EntityId attacker = 0;
	ska::EntityId target = 0;
	// The skill (its power, type, effects)
	const SkillDescriptor* skill = nullptr;
	// Normalized direction of the hit (it pushes the target that way), where it touched (world pixels)
	ska::Point<float> direction;
	ska::Point<float> impact;
};

// Applies a hit : damage (battle rules : statistics and their changes, types), the effects of the skill, and its feel
// (BattleStage) : everything stops a moment (hit stop), the target flashes, is stunned (it blinks), pushed back, the
// camera shakes, the damage appears. Returns the damage, -1 when the target dodged (a projectile goes on).
int ApplyBattleHit(ska::EntityManager& entities, PokemonGameEventDispatcher& ged, BattleRules& rules, BattleStage& stage, const BattleHit& hit);
