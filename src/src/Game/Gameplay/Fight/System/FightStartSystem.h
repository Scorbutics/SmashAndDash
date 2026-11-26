#pragma once
#include <unordered_set>
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "../FightComponent.h"
#include "../../Mobs/WildPokemonComponent.h"
#include "../../Control/GridMovementComponent.h"
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

// Starts a battle between the Pokemon of the player (the first of its team able to fight) and a wild Pokemon ("opponent", with
// a FightComponent). Returns false when no Pokemon of the team can fight (no battle).
bool StartBattle(CustomEntityManager& cem, PokemonGameEventDispatcher& ged, WorldState& ws, ska::EntityId trainer, ska::EntityId opponent);

// Starts a battle when the trainer and a wild Pokemon touch : one walks into the other (GridMovementComponent::touched).
// The attitude of each wild Pokemon drives its walk towards the player (WildPokemonComponent : it charges, it runs away),
// except while it is calm (ex : the player just fled from it).
class FightStartSystem : public ska::System< ska::RequiredComponent<ska::PositionComponent, FightComponent, WildPokemonComponent>, ska::PossibleComponent<GridMovementComponent>> {
public:
	FightStartSystem(CustomEntityManager& cem, PokemonGameEventDispatcher& ged, WorldState& ws, const ska::EntityLocator& entityLocator);
	virtual ~FightStartSystem();

	// A wild Pokemon left after a battle (the player fled or lost) ignores the player this long
	static constexpr unsigned int CalmAfterBattle = 5000;

protected:
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	CustomEntityManager& m_cem;
	WorldState& m_worldScene;
	const ska::EntityLocator& m_entityLocator;
	unsigned int m_t0;
	PokemonGameEventDispatcher& m_ged;
};


