#include <cstdlib>
#include "NumberUtils.h"

int NumberUtils::random(int min, int max) {
	return rand() % (max - min + 1) + min;
}

NumberUtils::NumberUtils() {}

NumberUtils::~NumberUtils() {}

float NumberUtils::random(float min, float max) {
	const double coeff = random();
	return (float)(coeff * (max - min + 1) + min);
}

double NumberUtils::random() {
	return rand() / ((double)RAND_MAX);
}

double NumberUtils::random(double min, double max) {
	const double coeff = random();
	return (coeff * (max - min + 1) + min);
}