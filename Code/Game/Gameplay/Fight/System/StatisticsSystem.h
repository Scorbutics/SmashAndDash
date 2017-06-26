#pragma once
#include <unordered_set>
#include "ECS/System.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "../BattleComponent.h"
#include "../../World/WorldState.h"
#include "../../PokemonGameEventDispatcher.h"

namespace ska {
	class Window;
}

class StatisticsSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, BattleComponent> {
public:
	StatisticsSystem(ska::EntityManager& em, ska::Window& w, ska::InputContextManager& icm, WorldState& ws, PokemonGameEventDispatcher& ged);
	StatisticsSystem(const StatisticsSystem&) = delete;
	StatisticsSystem operator=(const StatisticsSystem&) = delete;
	virtual ~StatisticsSystem();

protected:
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	ska::InputContextManager& m_playerICM;
	WorldState& m_worldScene;
	ska::Window& m_window;
	PokemonGameEventDispatcher& m_ged;
};
