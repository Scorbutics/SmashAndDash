#pragma once

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
		
		template <typename T>
		static T maximum(T a, T b) {
			return a > b ? a : b;
		}

		static double cosinus(double angle);
		static double sinus(double angle);

		static double absolute(double i);
		static unsigned int absolute(int i);

		static unsigned int getMax10Pow(const int num);
	};
}

