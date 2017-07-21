#pragma once
#include <unordered_set>
#include "ECS/Basics/Physic/PositionComponent.h"
#include "../FightComponent.h"
#include "ECS/System.h"
#include "Inputs/InputContextManager.h"
#include "Graphic/GraphicComponent.h"
#include "../../../Gameplay/PokemonGameEventDispatcher.h"
#include "AI/System/IADefinedMovementSystem.h"

class WorldState;
class CustomEntityManager;

namespace ska {
	class Window;
}

class FightStartSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::RequiredComponent<ska::PositionComponent, FightComponent, ska::GraphicComponent>, ska::PossibleComponent<>> {
public:
	FightStartSystem(CustomEntityManager& cem, ska::Window& w, PokemonGameEventDispatcher& ged, WorldState& ws, ska::InputContextManager& icm, const ska::EntityId player);
	virtual ~FightStartSystem();

protected:
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	CustomEntityManager& m_cem;
	WorldState& m_worldScene;
	ska::InputContextManager& m_icm;
	const ska::EntityId m_player;
	unsigned int m_t0;
	ska::Window& m_window;
	PokemonGameEventDispatcher& m_ged;
};


