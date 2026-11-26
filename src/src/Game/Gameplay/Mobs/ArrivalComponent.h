#pragma once
#include <algorithm>
#include <optional>
#include <string>
#include "Core/ECS/Component.h"
#include "Core/Point.h"

// A wild Pokemon arriving on the map or leaving it (ArrivalSystem) : only a drawing on a path (no physics body : it flies
// over the trees, it comes out of a bin), from a point to another (the positions of the corners of their blocks), in
// the air from a height to another. Arrived (Land, Emerge), it becomes a wild Pokemon on its block (WorldState::wildArrived) ;
// gone (TakeOff), it is removed. Its shadow stays on the ground, it is sorted by its feet (the depth of the map).
struct ArrivalComponent {
	// Land : it flies in from out of the screen and lands ; Emerge : it hops out of a hideout ; TakeOff : it flies away
	enum class Kind { Land, Emerge, TakeOff };
	Kind kind = Kind::Land;
	ska::Point<float> from;
	ska::Point<float> to;
	float fromHeight = 0.F;
	float toHeight = 0.F;
	// The height of a hop over the path (Emerge)
	float arc = 0.F;
	// Before it moves (ms) : the hideout rustles, it is not seen yet
	unsigned int wait = 0;
	unsigned int duration = 1000;
	unsigned int elapsed = 0;

	// The wild Pokemon it becomes (Land, Emerge)
	int monster = 0;
	int level = 1;
	// Its walk (the data of the map : 1, it wanders)
	int pathType = 0;
	std::string attitude = "passive";
	// A bird : how long it stays on the ground before it flies away (ms, Data/Battle/arrivals.miniska "stay")
	std::optional<int> stay;
	// The block of the hideout it comes out of (Emerge)
	std::optional<ska::Point<int>> hideout;

	bool moving() const { return elapsed >= wait; }
	// 0 .. 1 along its path
	float progress() const { return moving() ? std::min(1.F, static_cast<float>(elapsed - wait) / static_cast<float>(duration)) : 0.F; }
};

SKA_DECLARE_COMPONENT(ArrivalComponent);
