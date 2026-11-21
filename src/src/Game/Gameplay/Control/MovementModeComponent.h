#pragma once
#include <string>
#include "Core/ECS/Component.h"

// Movement mode of a character controlled by the player (Character.setMovement) : "walk", "run" or "locked".
// The powers of its normal walk are kept : the modes are computed from them.
struct MovementModeComponent {
	std::string mode = "walk";
	float walkPower = 0.F;
	float jumpPower = 0.F;
};

SKA_DECLARE_COMPONENT(MovementModeComponent);
