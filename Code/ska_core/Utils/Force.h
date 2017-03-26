#pragma once

#include "../Rectangle.h"

namespace ska {
	struct Force {
		Force() : pos({ 0, 0, 0, 0 }), angle(0), power(0) {
		}

		Force(Rectangle p, float pow, float a) :
			pos(p),
			angle(a),
			power(pow) {
		}

		~Force() = default;

		Rectangle pos;
		float angle;
		float power;

	};
}

