#pragma once
#include <unordered_set>
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "../FightComponent.h"
#include "Core/ECS/System.h"
#include "Core/Inputs/InputContextManager.h"
#include "Graphic/GraphicComponent.h"
#include "../../../Gameplay/PokemonGameEventDispatcher.h"
#include "AI/System/IADefinedMovementSystem.h"

class WorldState;
class CustomEntityManager;

namespace ska {
	class Window;
	class EntityLocator;
}

class SavegameManager;

// Starts a battle between the Pokemon of the player (the first of its team) and a wild Pokemon ("opponent", with a FightComponent)
void StartBattle(CustomEntityManager& cem, PokemonGameEventDispatcher& ged, WorldState& ws, ska::EntityId trainer, ska::EntityId opponent);

// Starts a battle sometimes, when a wild Pokemon is close to the player
class FightStartSystem : public ska::System< ska::RequiredComponent<ska::PositionComponent, FightComponent, ska::GraphicComponent>, ska::PossibleComponent<>> {
public:
	FightStartSystem(CustomEntityManager& cem, PokemonGameEventDispatcher& ged, WorldState& ws, const ska::EntityLocator& entityLocator);
	virtual ~FightStartSystem();

protected:
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	CustomEntityManager& m_cem;
	WorldState& m_worldScene;
	const ska::EntityLocator& m_entityLocator;
	unsigned int m_t0;
	PokemonGameEventDispatcher& m_ged;
};


