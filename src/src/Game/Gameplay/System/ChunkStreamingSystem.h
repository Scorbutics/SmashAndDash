#pragma once
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "../World/WorldState.h"

// The chunks around the player while the map goes on : they come before it reaches them, they leave behind it
// (WorldStreamer). Not during the battles : the map is as it was when they started.
class ChunkStreamingSystem : public ska::System<ska::RequiredComponent<ska::PositionComponent>, ska::PossibleComponent<>> {
public:
	ChunkStreamingSystem(ska::EntityManager& entityManager, WorldState& worldState) : System(entityManager), m_worldState(worldState) {}
	~ChunkStreamingSystem() override = default;

protected:
	void refresh(unsigned int ellapsedTime) override { m_worldState.updateStreaming(ellapsedTime); }

private:
	WorldState& m_worldState;
};
