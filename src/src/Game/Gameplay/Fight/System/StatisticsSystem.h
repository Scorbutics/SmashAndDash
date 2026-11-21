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
	// The battle ends when a Pokemon has no hit points left : won when it is the opponent
	StatisticsSystem(ska::EntityManager& em, WorldState& ws, PokemonGameEventDispatcher& ged, ska::EntityId pokemon, ska::EntityId opponent);
	StatisticsSystem(const StatisticsSystem&) = delete;
	StatisticsSystem operator=(const StatisticsSystem&) = delete;
	virtual ~StatisticsSystem();

protected:
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	ska::EntityManager& m_entities;
	WorldState& m_worldScene;
	PokemonGameEventDispatcher& m_ged;
	ska::EntityId m_pokemon;
	ska::EntityId m_opponent;
	bool m_ended = false;
};
