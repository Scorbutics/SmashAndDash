#pragma once
#include "../Exceptions/IllegalArgumentException.h"
#include "../Graphic/Point.h"

namespace ska{
	class NumberUtils
	{

	private:
		NumberUtils();

	public:
		~NumberUtils();

		static float random(float min, float max);
		static double random(double min, double max);
		static double random();
		static float exponential(float i);
		static int random(int min, int max);
		static float arctan(float slope);
		static double arctan(double slope);

		template <typename T>
		static T maximum(T a, T b) {
			return a > b ? a : b;
		}

		static double cosinus(double angle);
		static double sinus(double angle);

		static double absolute(double i);
		static unsigned int absolute(int i);

		static unsigned int squareroot(const unsigned int i);
		static unsigned int getMax10Pow(const int num);

		template <typename T>
		static Point<T> cartesian(const T radius, const double angle) {
			Point<T> result;
			result.x = radius * (float) cosinus(angle);
			result.y = radius * (float) sinus(angle);
			return result;
		}

		template <typename T>
		static PolarPoint<T> polar(const T x, const T y) {
			PolarPoint<T> result;
			if (y == 0) {
				throw ska::IllegalArgumentException("Error while converting cartesian coordinates to polar : slope cannot be infinite (x must be != 0)");
			}

			result.angle = artcan(y / x);
			result.radius = squareroot(x * x + y * y);
			return result;
		}
	};
}

