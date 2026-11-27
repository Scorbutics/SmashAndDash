#pragma once
#include <vector>
#include "Core/ECS/ECSDefines.h"
#include "../PokemonGameEventDispatcher.h"

namespace ska {
	class EntityManager;
}
class Bag;
class BattleStage;
class WorldState;

// The items of the trainer in a battle, used in real time (TrainerItemSystem : E, R) or as the action of a round of a
// clash (ClashSystem : the bag of its menu)

// The items of the bag usable in a battle (a healing item, a pokeball), in the order of the bag
std::vector<int> BattleItems(const Bag& bag);

// A healing item on a fighter : the hit points given (0 : no effect, the item is kept ; else it is used)
int HealFighter(ska::EntityManager& entities, BattleStage& stage, Bag& bag, int item, int amount, ska::EntityId fighter);

// A pokeball thrown at a wild Pokemon : the dice (rolled by the battle), the chance (the battle rules), the shakes of the
// ball (3 : caught ; fewer the further the roll was)
struct CaptureRoll {
	bool caught = false;
	int shakes = 0;
	int chance = 0;
	int roll = 0;
};
CaptureRoll RollCapture(ska::EntityManager& entities, WorldState& worldState, ska::EntityId target);

// The wild Pokemon is captured : out of the fight, it joins the team (its level, its hit points left), the battle is won
// by the capture when no other wild Pokemon stands (BattleDirectorSystem)
void CaptureFighter(ska::EntityManager& entities, PokemonGameEventDispatcher& ged, WorldState& worldState, BattleStage& stage, ska::EntityId target);
