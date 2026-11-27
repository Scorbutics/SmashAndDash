#pragma once
#include <optional>
#include <vector>
#include "Core/ECS/Component.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"

// A character that moves on the map (GridMovementSystem) : block by block, or freely (the player)
struct GridMovementComponent {
	// Keys : the player (with an InputComponent), freely (in 8 directions, the walls of the physics). Wander : random steps after random waits. Path : the directions of "path",
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
	// The player (Keys) walks freely, not block by block : its velocity (pixels per second), towards the keys held
	ska::Point<float> velocity;
	// Its gait : 0 still, 1 walking, 2 running ; a pose between two gaits (AnimationComponent::pose : setting off, pushing
	// off, braking) is shown until "poseUntil" (ticks). The animation follows them (the links of WorldState)
	int gait = 0;
	unsigned int poseUntil = 0;
	// Direction of the wall it walks against (-1 : none) : the wall is touched once (its scripts, ex : the water to surf)
	int bumpDirection = -1;
	// The character its last step walked into (refused : that character stands on the block), 0 : none. Set again at each
	// logic update (ex : the trainer walks into a wild Pokemon : a battle starts). Entity ids start at 0 (the trainer) : none is
	// "no value"
	std::optional<ska::EntityId> touched;

	// Wander : a character in sight ("reactTo", within "sight" blocks) is approached or avoided instead (the wild
	// Pokemon : their attitude), with steps of "reactStep" ms and short waits
	enum class Reaction { None, Approach, Avoid };
	Reaction reaction = Reaction::None;
	std::optional<ska::EntityId> reactTo;
	int sight = 0;
	unsigned int reactStep = 220;
	// It sees the character it reacts to : since "seen" ms (the mark over it when it notices it)
	bool seeing = false;
	unsigned int seen = 0;

	// Steps asked by a script or by the game (ex : the jump over a ledge, leaving a door) : block by block, through the
	// obstacles, without the keys. The character is a ghost during these steps (no physics contact, no touched wall).
	int forcedSteps = 0;
	int forcedDirection = 0;
	unsigned int forcedDuration = 0;
	bool forcedGhost = false;
	bool savedGhost = false;

	// A hop (the jump key, the jump over a ledge) : the height of the character follows an arc, from the ground to
	// "hopHeight" (pixels) and back, over "hopDuration" ms. Not the physics : the same height whatever the frame rate.
	unsigned int hopDuration = 0;
	unsigned int hopElapsed = 0;
	float hopHeight = 0.F;
};

SKA_DECLARE_COMPONENT(GridMovementComponent);
