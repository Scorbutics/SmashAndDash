#pragma once
#include <vector>
#include "Core/ECS/Component.h"
#include "Core/Point.h"

// A character that moves block by block on the map (GridMovementSystem)
struct GridMovementComponent {
	// Keys : the player (with an InputComponent). Wander : random steps after random waits. Path : the directions of "path",
	// again and again.
	enum class Mode { Keys, Wander, Path };
	Mode mode = Mode::Keys;
	std::vector<int> path;
	std::size_t pathIndex = 0;
	// Time before the next step of a character not controlled by the player
	unsigned int wait = 0;

	// Direction it faces (0 : down, 1 : right, 2 : up, 3 : left)
	int facing = 0;
	// Step in progress : from a position to the next block (positions of the PositionComponent)
	bool stepping = false;
	int stepDirection = 0;
	ska::Point<float> from;
	ska::Point<float> to;
	unsigned int elapsed = 0;
	unsigned int duration = 0;
	// The key was pressed to turn : it walks once held long enough
	bool turning = false;
	unsigned int heldTime = 0;
	// Direction of the wall it walks against (-1 : none) : the wall is touched once (its scripts, ex : the water to surf)
	int bumpDirection = -1;
};

SKA_DECLARE_COMPONENT(GridMovementComponent);
