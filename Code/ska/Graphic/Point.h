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

		Point<T> operator-(const Point<T>& p) const {
			Point<T> result = *this;
			result.x -= p.x;
			result.y -= p.y;
			return result;
		}

		Point<T> operator+(const Point<T>& p) const {
			Point<T> result = *this;
			result.x += p.x;
			result.y += p.y;
			return result;
		}

		void operator+=(const Point<T>& p) const {
			*this = operator+(p);
		}

		void operator-=(const Point<T>& p) const {
			*this = operator-(p);
		}
	};


	template <class T>
	struct PolarPoint {
		T radius;
		float angle;

		PolarPoint<T>(T radius, float angle){ this->radius = radius; this->angle = angle; }

		PolarPoint<T>(){ radius = 0; angle = 0; }

		template <class U>
		PolarPoint<T>(const U& r) {
			radius = r.radius;
			angle = r.angle;
		}

		template <class U>
		PolarPoint<T>(const PolarPoint<U>& p) {
			*this = p;
		}

		template <class U>
		void operator=(const PolarPoint<U>& p) {
			radius = (T) p.radius;
			angle = p.angle;
		}

	};
}

