#pragma once

#include "Rectangle.h"

namespace ska {

	template <class T>
	struct Point {
		T x, y;


		Point<T>(T x, T y){ this->x = x; this->y = y; }

		Point<T>(){ x = 0; y = 0; }

		template <class U>
		Point<T>(const U& r) {
			x = r.x;
			y = r.y;
		}

		template <class U>
		Point<T>(const Point<U>& p) {
			*this = p;
		}

		template <class U>
		void operator=(const Point<U>& p) {
			x = (T)p.x;
			y = (T)p.y;
		}

	};
}

