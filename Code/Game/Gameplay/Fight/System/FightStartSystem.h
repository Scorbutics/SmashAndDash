#pragma once
#include <unordered_set>
#include "ECS/Basics/Physic/PositionComponent.h"
#include "../FightComponent.h"
#include "ECS/System.h"
#include "Core/Scene/SceneHolder.h"
#include "Inputs/InputContextManager.h"
#include "Graphic/GraphicComponent.h"
#include "../../../Gameplay/PokemonGameEventDispatcher.h"

class WorldScene;
class CustomEntityManager;

namespace ska {
	class Window;
}

class FightStartSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, FightComponent, ska::GraphicComponent> {
public:
	FightStartSystem(CustomEntityManager& cem, ska::Window& w, PokemonGameEventDispatcher& ged, WorldScene& ws, ska::InputContextManager& icm, const ska::EntityId player);
	virtual ~FightStartSystem();

protected:
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	CustomEntityManager& m_cem;
	WorldScene& m_worldScene;
	ska::InputContextManager& m_icm;
	//ska::SceneHolder& m_sceneHolder;
	const ska::EntityId m_player;
	unsigned int m_t0;
	ska::Window& m_window;
	PokemonGameEventDispatcher& m_ged;
};


