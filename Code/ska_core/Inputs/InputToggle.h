#pragma once

#include <bitset>

namespace ska {
	enum InputToggle {
		MoveWindow,
		MoveTo,
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		Run,
	};

#define INPUT_MAX_TOGGLE 16

	typedef std::bitset<INPUT_MAX_TOGGLE> InputToggleContainer;
}
