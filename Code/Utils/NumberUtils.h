#pragma once
class NumberUtils
{

public:
	NumberUtils();
	~NumberUtils();

	static float random(float min, float max);
	static double random(double min, double max);
	static double random();
	static int random(int min, int max);

};

