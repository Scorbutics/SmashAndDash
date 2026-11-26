#pragma once
#include <functional>
#include <utility>
#include "Core/Point.h"
#include "Core/ECS/ECSDefines.h"
#include "Base/Task/Task.h"

class CustomEntityManager;

// The trainer throws a pokeball (PokeballSystem) : the task ends once it landed, opened, and the light lets the Pokemon
// come out ("landing" : where it opened). "path" gives the points of the throw on the ground when it starts (from, to :
// world pixels).
class PokeballTransition : public ska::Task {
public:
	using Path = std::function<std::pair<ska::Point<float>, ska::Point<float>>()>;
	PokeballTransition(CustomEntityManager& em, Path path, ska::Point<float>& landing);
	~PokeballTransition() override = default;

	// The Pokemon comes out this long after the pokeball opened (the light grows)
	static constexpr unsigned int AppearDelay = 120;
	// The longest throw : the Pokemon comes out anyway (the battle never waits for a lost pokeball)
	static constexpr unsigned int MaxDuration = 3000;

private:
	bool run(ska::Task& task);

	CustomEntityManager& m_entityManager;
	Path m_path;
	ska::Point<float>& m_landing;
	ska::EntityId m_ball = 0;
	bool m_thrown = false;
	unsigned int m_thrownAt = 0;
	unsigned int m_openedAt = 0;
};
