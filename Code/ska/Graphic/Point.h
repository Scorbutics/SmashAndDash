#pragma once

#include "Rectangle.h"

namespace ska {

	template <class T>
	struct Point {
		T x, y;


		Point<T>(T x, T y){ this->x = x; this->y = y; }

		Point<T>(){ x = 0; y = 0; }

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
		void operator=(const Point<U>& p) {
			x = static_cast<T>(p.x);
			y = static_cast<T>(p.y);
		}

		bool operator==(const Point<T>& p) const {
			return x == p.x && y == p.y;
		}

		bool operator!=(const Point<T>& p) const {
			return !(operator==(p));
		}

		Point<T> operator-(const Point<T>& p) const {
			Point<T> result = *this;
			result.x -= p.x;
			result.y -= p.y;
			return result;
		}

		Point<T> operator/(const unsigned int i) const {
			Point<T> result = *this;
			result.x /= i;
			result.y /= i;
			return result;
		}

		Point<T> operator*(const unsigned int i) const {
			Point<T> result = *this;
			result.x *= i;
			result.y *= i;
			return result;
		}

		Point<T> operator+(const Point<T>& p) const {
			Point<T> result = *this;
			result.x += p.x;
			result.y += p.y;
			return result;
		}

		void operator+=(const Point<T>& p) {
			*this = operator+(p);
		}

		void operator-=(const Point<T>& p) {
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
		PolarPoint(const U& r);

		template <class U>
		PolarPoint(const PolarPoint<U>& p);

		template <class U>
		void operator=(const PolarPoint<U>& p) {
			radius = static_cast<T>(p.radius);
			angle = p.angle;
		}

	};

	template <class T>
	Point<T>::Point(const int& r){
		x = r;
		y = r;
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
		*this = p;
	}
}

