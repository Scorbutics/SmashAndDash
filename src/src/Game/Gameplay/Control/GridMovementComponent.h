#pragma once
#include "Core/ECS/Component.h"
#include "Core/Point.h"

// A character controlled by the player that moves block by block on the map (GridMovementSystem)
struct GridMovementComponent {
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
};

SKA_DECLARE_COMPONENT(GridMovementComponent);
