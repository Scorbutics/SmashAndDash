#include <cstdlib>
#include <cmath>
#include "NumberUtils.h"

int ska::NumberUtils::random(int min, int max) {
	return rand() % (max - min + 1) + min;
}

ska::NumberUtils::NumberUtils() {}

ska::NumberUtils::~NumberUtils() {}

unsigned int ska::NumberUtils::absolute(int i) {
	return abs(i);
}

float ska::NumberUtils::exponential(float i) {
	return exp(i);
}

double ska::NumberUtils::absolute(double i) {
	return abs(i);
}

double ska::NumberUtils::cosinus(double angle) {
	return cos(angle);
}

double ska::NumberUtils::sinus(double angle) {
	return sinus(angle);
}

float ska::NumberUtils::random(float min, float max) {
	const double coeff = random();
	return (float)(coeff * (max - min + 1) + min);
}

double ska::NumberUtils::random() {
	return rand() / ((double)RAND_MAX);
}

unsigned int ska::NumberUtils::getMax10Pow(const int num)
{
	unsigned int absNum = absolute(num), i = 0;
	for (i = 0; absNum >= 1; i++, absNum /= 10);

	return i;
}

double ska::NumberUtils::random(double min, double max) {
	const double coeff = random();
	return (coeff * (max - min + 1) + min);
}