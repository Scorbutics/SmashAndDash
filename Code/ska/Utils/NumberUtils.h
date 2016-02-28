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
		static int random(int min, int max);

		static double absolute(double i);
		static unsigned int absolute(int i);

		static unsigned int getMax10Pow(const int num);
	};
}

