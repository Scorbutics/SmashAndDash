#pragma once
#include <vector>
#include "../Point.h"

namespace ska {
	enum InputRangeType {
		MousePos,
		LastMousePos
	};

	typedef Point<float> InputRange;

	typedef std::vector<InputRange> InputRangeContainer;

#define INPUT_MAX_RANGE 2
}
