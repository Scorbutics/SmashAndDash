#pragma once
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"
#include "../PokemonGameEventDispatcher.h"

namespace ska {
	class EntityManager;
}
class BattleRules;
class BattleStage;

// A skill touching a fighter (a projectile, or a contact skill as Charge)
struct BattleHit {
	ska::EntityId attacker = 0;
	ska::EntityId target = 0;
	int power = 0;
	bool special = false;
	int knockback = 0;
	// Normalized direction of the hit (it pushes the target that way), where it touched (world pixels)
	ska::Point<float> direction;
	ska::Point<float> impact;
};

// Applies a hit : damage (battle rules), and its feel (BattleStage) : everything stops a moment (hit stop), the target
// flashes, is stunned (it blinks), pushed back, the camera shakes, the damage appears. Returns the damage.
int ApplyBattleHit(ska::EntityManager& entities, PokemonGameEventDispatcher& ged, BattleRules& rules, BattleStage& stage, const BattleHit& hit);
