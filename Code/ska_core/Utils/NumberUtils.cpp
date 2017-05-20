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
	return sin(angle);
}

double ska::NumberUtils::arctan(int x, int y) {
	return atan2(y, x);
}

bool ska::NumberUtils::isLowValue(double v, double threshold) {
	return absolute(v) < threshold;
}

int ska::NumberUtils::calculateSlopeSign(int dest, int src) {
	return dest == src ? 0 : ((dest - src) / absolute(dest - src));
}

double ska::NumberUtils::arctan(double slope) {
	return atan(slope);
}

float ska::NumberUtils::random(float min, float max) {
	const double coeff = random();
	return static_cast<float>(coeff * (max - min + 1) + min);
}

unsigned int ska::NumberUtils::squarerooti(const unsigned int i) {
	return static_cast<unsigned int>(sqrt(i));
}

double ska::NumberUtils::squareroot(const double i) {
	return sqrt(i);
}

double ska::NumberUtils::random() {
	return rand() / static_cast<double>(RAND_MAX);
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
