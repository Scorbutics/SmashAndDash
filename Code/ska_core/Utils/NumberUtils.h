#pragma once
#include <functional>
#include "../Exceptions/IllegalArgumentException.h"

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

		template <class T>
		static void hashCombine(size_t& seed, T& value) {
			seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		static float exponential(float i);
		static int random(int min, int max);
		static double arctan(int x, int y);
		static double arctan(double slope);

		template <typename T>
		static T maximum(T a, T b) {
			return a > b ? a : b;
		}

		template <typename T>
		static T minimum(T a, T b) {
			return a < b ? a : b;
		}

		static double cosinus(double angle);
		static double sinus(double angle);

		static double absolute(double i);
		static unsigned int absolute(int i);

		static unsigned int squareroot(const unsigned int i);
		static unsigned int getMax10Pow(const int num);

		static bool isLowValue(double v, double threshold);

		static int calculateSlopeSign(int dest, int src);

	};
}

