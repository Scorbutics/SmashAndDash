#include <cstdlib>
#include <cmath>
#include "NumberUtils.h"
#include <SDL_stdinc.h>

#define SKA_MATH_OPTIMIZATIONS

int ska::NumberUtils::random(int min, int max) {
	return rand() % (max - min + 1) + min;
}

ska::NumberUtils::NumberUtils() {}

ska::NumberUtils::~NumberUtils() {}

unsigned int ska::NumberUtils::absolute(int i) {
	return abs(i);
}

int ska::NumberUtils::round(float f) {
	return static_cast<int>(f > 0 ? f + 0.5 : f - 0.5);
}


float ska::NumberUtils::exponential(float i) {
	return exp(i);
}

double ska::NumberUtils::absolute(double i) {
	return abs(i);
}

double ska::NumberUtils::cosinus(double angle) {
#ifdef SKA_MATH_OPTIMIZATIONS
	return fastSin((M_PI / 2) - angle);
#else
	return cos(angle);
#endif
}

float ska::NumberUtils::fastSin(float x) {
	// restrict x so that -M_PI < x < M_PI
	x = fmod(x + M_PI, M_PI * 2) - M_PI;
	const float B = 4.0f / M_PI;
	const float C = -4.0f / (M_PI*M_PI);

	auto y = B * x + C * x * std::abs(x);

	const auto P = 0.225f;

	return P * (y * std::abs(y) - y) + y;
}

double ska::NumberUtils::sinus(double angle) {
#ifdef SKA_MATH_OPTIMIZATIONS
	return fastSin(angle);
#else
	return sin(angle);
#endif
}

double ska::NumberUtils::arctan(int x, int y) {
#ifdef SKA_MATH_OPTIMIZATIONS
	return fastAtan2(y, x);
#else
	return atan2(y, x);
#endif
}

bool ska::NumberUtils::isLowValue(double v, double threshold) {
	return absolute(v) < threshold;
}

int ska::NumberUtils::calculateSlopeSign(int dest, int src) {
	return dest == src ? 0 : ((dest - src) / absolute(dest - src));
}

float ska::NumberUtils::fastAtan2(float y, float x) {
	//http://pubs.opengroup.org/onlinepubs/009695399/functions/atan2.html
	//Volkan SALMA

	const float ONEQTR_PI = M_PI / 4.0;
	const float THRQTR_PI = 3.0 * M_PI / 4.0;
	float r, angle;
	float abs_y = fabs(y) + 1e-10f;      // kludge to prevent 0/0 condition
	if (x < 0.0f)
	{
		r = (x + abs_y) / (abs_y - x);
		angle = THRQTR_PI;
	}
	else
	{
		r = (x - abs_y) / (x + abs_y);
		angle = ONEQTR_PI;
	}
	angle += (0.1963f * r * r - 0.9817f) * r;
	if (y < 0.0f)
		return(-angle);     // negate if in quad III or IV
	else
		return(angle);


}

/*double ska::NumberUtils::arctan(double slope) {
	return atan(slope);
}*/

float ska::NumberUtils::random(float min, float max) {
	const auto coeff = random();
	return static_cast<float>(coeff * (max - min + 1) + min);
}

extern "C" {
    //Pasted from Greg Walsh
    float ComputeFastInverseSquareRoot(float x) {
        union {
            float x;
            int i;
        } u;

        float xhalf = 0.5f * x;
        u.x = x;
        u.i = 0x5f3759df - (u.i >> 1);
        u.x = u.x * (1.5f - xhalf * u.x * u.x);
        return u.x;
    }

}

float ska::NumberUtils::fastInverseSquareroot(float number) {
    return ComputeFastInverseSquareRoot(number);
}

double ska::NumberUtils::squareroot(const double i) {
#ifdef SKA_MATH_OPTIMIZATIONS
	return i * fastInverseSquareroot(i);
#else
	return sqrt(i);
#endif

}

// http://www.rgba.org/articles/sfrand/sfrand.htm
static unsigned int mirand = 1;
float sfrand(void) {
    unsigned int a;
    mirand *= 16807;
    a = (mirand & 0x007fffff) | 0x40000000;
    return(*((float*)&a) - 3.0f);
}

double ska::NumberUtils::random() {
    #ifdef SKA_MATH_OPTIMIZATIONS
    return sfrand() / static_cast<double>(RAND_MAX);
    #else
    return rand() / static_cast<double>(RAND_MAX);
    #endif // SKA_MATH_OPTIMIZATIONS

}

unsigned int ska::NumberUtils::getMax10Pow(const int num) {
	unsigned int absNum = absolute(num), i = 0;
	for (i = 0; absNum >= 1; i++, absNum /= 10);

	return i;
}

double ska::NumberUtils::random(double min, double max) {
	const auto coeff = random();
	return (coeff * (max - min + 1) + min);
}
