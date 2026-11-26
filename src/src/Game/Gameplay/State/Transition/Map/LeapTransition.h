#pragma once
#include "Core/Point.h"
#include "Core/ECS/ECSDefines.h"
#include "Base/Task/Task.h"

class CustomEntityManager;
class WorldState;

// A character hops away from another one, block by block in a straight line (at most "blocks", it stops before a wall),
// and faces it : the wild Pokemon touched by the trainer jumps back, the room of the battle.
class LeapTransition : public ska::Task {
public:
	LeapTransition(CustomEntityManager& em, WorldState& ws, ska::EntityId jumper, ska::EntityId awayFrom, int blocks);
	~LeapTransition() override = default;

	static constexpr unsigned int Duration = 380;
	static constexpr float Height = 22.F;

private:
	bool run(ska::Task& task);
	void start();

	CustomEntityManager& m_entityManager;
	WorldState& m_worldState;
	ska::EntityId m_jumper;
	ska::EntityId m_awayFrom;
	int m_blocks;
	bool m_started = false;
	unsigned int m_startedAt = 0;
	ska::Point<float> m_from;
	ska::Point<float> m_to;
};
