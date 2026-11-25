#pragma once
#include <cstdint>

// The noise of the world generator : the C++ port of tools/worldgen (and tools/tilegen h01). The same numbers as the
// Python generator, bit for bit : the same seed gives the same world (tests/worldgen : the reference values).
namespace worldgen {
	// A hash of a position in [0, 1) : the same everywhere (64 bits arithmetic, its low 32 bits as in Python)
	double H01(std::int64_t x, std::int64_t y, std::int64_t seed = 0);
	// Smooth step
	double Smooth(double t);
	// Value noise, continuous everywhere ; fractal noise of "octaves" octaves (in [0, 1])
	double ValueNoise(double x, double y, std::int64_t seed);
	double Fbm(double x, double y, std::int64_t seed, int octaves = 4);
}
