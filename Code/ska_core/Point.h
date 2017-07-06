#pragma once

#include "Utils/NumberUtils.h"
#include "Rectangle.h"

namespace ska {

	template <class T>
	struct Point {
		T x, y;


		Point<T>(T x, T y){ this->x = x; this->y = y; }

		Point<T>(){ x = 0; y = 0; }

		Point(const Point<T>&) = default;
		Point(Point<T>&&) = default;

		Point<T>& operator=(Point<T>&&) = default;

        Point<T>& operator=(const Point<T>& p) {
            x = static_cast<T>(p.x);
            y = static_cast<T>(p.y);
            return *this;
        }


		template <class U>
		Point(const U& r) {
			x = r.x;
			y = r.y;
		}

		Point<T>(const int& r);

		template <class U>
		Point(const Point<U>& p);

		Point<T>(const Rectangle& r) {
			x = r.x;
			y = r.y;
		}

		template <class U>
		Point<T>& operator=(const Point<U>& p) {
			x = static_cast<T>(p.x);
			y = static_cast<T>(p.y);
			return *this;
		}

		bool operator==(const Point<T>& p) const {
			return x == p.x && y == p.y;
		}

		bool operator!=(const Point<T>& p) const {
			return !(operator==(p));
		}

		Point<T> operator-(const Point<T>& p) const {
			return Point<T>(x - p.x, y - p.y);
		}

		Point<T> operator/(const unsigned int i) const {
			return Point<T>(x / i, y / i);
		}

		Point<T> operator*(const unsigned int i) const {
			return Point<T>(x * i, y * i);
		}

		Point<T> operator+(const Point<T>& p) const {
			return Point<T>(x + p.x, y + p.y);
		}

		void operator+=(const Point<T>& p) {
			x += p.x;
			y += p.y;
		}

		void operator-=(const Point<T>& p) {
			x -= p.x;
			y -= p.y;
		}

		static Point<T> cartesian(const T radius, const double angle) {
			return Point<T>(radius * static_cast<float>(ska::NumberUtils::cosinus(angle)),
							radius * static_cast<float>(ska::NumberUtils::sinus(angle)));
		}

		/*
			In a non-standard coordinate system (as we are, in computer graphics we have the y axis down and not up),
			Matrix rotation equation
			|x'|   |cos(angle)	sin(angle)	| * |x|
			|y'| = |-sin(angle)	cos(angle)	|	|y|
			with (x'; y') the resulting coords, (x; y) the current ones and angle the angle of rotation
		*/
		static Point<T> rotate(const Point<T>& origin, double angle, const Point<T>& currentPoint) {
			/* First, we have to work with a (0;0) origin to make the rotation correctly */
			const Point<T> diff(currentPoint.x - origin.x,
								currentPoint.y - origin.y);

			/* Then we apply the multiplication with the rotation matrix with angle "angle" */
			Point<T> result(static_cast<T>(diff.x * ska::NumberUtils::cosinus(angle) + diff.y * ska::NumberUtils::sinus(angle)),
							static_cast<T>(- diff.x * ska::NumberUtils::sinus(angle) + diff.y * ska::NumberUtils::cosinus(angle)));

			result.y = -result.y;

			/* And we add the origin again */
			return result + origin;
		}
	};


	template <class T>
	struct PolarPoint {
		T radius;
		float angle;

		PolarPoint<T>(T radius, float angle){ this->radius = radius; this->angle = angle; }

		PolarPoint<T>(){ radius = 0; angle = 0; }

		template <class U>
		PolarPoint(const U& r);

		template <class U>
		PolarPoint(const PolarPoint<U>& p);

		template <class U>
		void operator=(const PolarPoint<U>& p) {
			radius = static_cast<T>(p.radius);
			angle = p.angle;
		}

		template<class U>
		static PolarPoint<T> polar(const U x, const U y) {
			return std::move(PolarPoint<T> (
				NumberUtils::squareroot(static_cast<double>(x * x + y * y)),
				static_cast<float>(NumberUtils::arctan(x, y))));
		}
	};

	template<>
    inline bool Point<float>::operator==(const Point<float>& p) const {
        return NumberUtils::isLowValue(x - p.x, 0.01) && NumberUtils::isLowValue(y - p.y, 0.01);
    }

	template<>
    inline bool Point<double>::operator==(const Point<double>& p) const {
        return NumberUtils::isLowValue(x - p.x, 0.01) && NumberUtils::isLowValue(y - p.y, 0.01);
	}

	template <class T>
	Point<T>::Point(const int& r){
		x = static_cast<T>(r);
		y = static_cast<T>(r);
	}

	template <class T>
	template <class U>
	Point<T>::Point(const Point<U>& p){
		*this = p;
	}

	template <class T>
	template <class U>
	PolarPoint<T>::PolarPoint(const U& r){
		radius = r.radius;
		angle = r.angle;
	}

	template <class T>
	template <class U>
	PolarPoint<T>::PolarPoint(const PolarPoint<U>& p){
		radius = p.radius;
		angle = p.angle;
	}
}

