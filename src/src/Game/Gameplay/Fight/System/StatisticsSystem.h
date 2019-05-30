#pragma once
#include <unordered_set>
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "../BattleComponent.h"
#include "../../World/WorldState.h"
#include "../../PokemonGameEventDispatcher.h"
#include "AI/System/IADefinedMovementSystem.h"

namespace ska {
	class Window;
}

class StatisticsSystem : public ska::System< ska::RequiredComponent<ska::PositionComponent, BattleComponent>, ska::PossibleComponent<>> {
public:
	StatisticsSystem(ska::EntityManager& em, WorldState& ws, PokemonGameEventDispatcher& ged);
	StatisticsSystem(const StatisticsSystem&) = delete;
	StatisticsSystem operator=(const StatisticsSystem&) = delete;
	virtual ~StatisticsSystem();

protected:
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	WorldState& m_worldScene;
	PokemonGameEventDispatcher& m_ged;
};
