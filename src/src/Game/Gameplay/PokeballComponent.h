#pragma once
#include <optional>
#include "Core/ECS/Component.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"

// A pokeball thrown (PokeballSystem) : from a point on the ground to another, over an arc. Thrown out, it bounces where
// it lands and opens (a burst of light : the Pokemon comes out). Thrown back ("returning" : the recall of a Pokemon), it
// disappears in the hands of the trainer. Thrown at a wild Pokemon ("capture") : it lands, shakes "shakes" times, then it
// is "caught" (the ball clicks, glows and fades) or it opens (the Pokemon breaks free).
class PokeballComponent {
public:
	enum class State { Flying, Bouncing, Shaking, Caught, Open, Gone };

	// Points on the ground (world pixels), the flight time (ms) and the height of its arc (pixels)
	ska::Point<float> from;
	ska::Point<float> to;
	unsigned int duration = 600;
	float height = 56.F;
	bool returning = false;
	bool capture = false;
	int shakes = 0;
	bool caught = false;

	State state = State::Flying;
	unsigned int elapsed = 0;
	// The burst of light of the opening (an entity)
	std::optional<ska::EntityId> burst;

	// It landed and opened : the Pokemon comes out
	bool opened() const { return state == State::Open || state == State::Gone; }
	// Its course is over (opened and faded, or back in the hands of the trainer)
	bool done() const { return state == State::Gone; }
};

SKA_DECLARE_COMPONENT(PokeballComponent);
