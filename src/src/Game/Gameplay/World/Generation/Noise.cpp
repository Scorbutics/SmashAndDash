#include <cmath>
#include "Noise.h"

double worldgen::H01(std::int64_t x, std::int64_t y, std::int64_t seed) {
	// The low 32 bits of the exact products : a wrapping 64 bits arithmetic keeps them
	const auto ux = static_cast<std::uint64_t>(x);
	const auto uy = static_cast<std::uint64_t>(y);
	const auto us = static_cast<std::uint64_t>(seed);
	auto n = (ux * 374761393ULL + uy * 668265263ULL + us * 2147483647ULL) & 0xFFFFFFFFULL;
	n = ((n ^ (n >> 13)) * 1274126177ULL) & 0xFFFFFFFFULL;
	return static_cast<double>((n ^ (n >> 16)) & 0xFFFFULL) / 65536.0;
}

double worldgen::Smooth(double t) {
	return t * t * (3 - 2 * t);
}

namespace {
	double Lattice(std::int64_t ix, std::int64_t iy, std::int64_t seed) {
		return worldgen::H01(ix, iy, seed * 7919 + 13);
	}
}

double worldgen::ValueNoise(double x, double y, std::int64_t seed) {
	const auto ix = static_cast<std::int64_t>(std::floor(x));
	const auto iy = static_cast<std::int64_t>(std::floor(y));
	const auto fx = Smooth(x - static_cast<double>(ix));
	const auto fy = Smooth(y - static_cast<double>(iy));
	const auto a = Lattice(ix, iy, seed);
	const auto b = Lattice(ix + 1, iy, seed);
	const auto c = Lattice(ix, iy + 1, seed);
	const auto d = Lattice(ix + 1, iy + 1, seed);
	return (a + (b - a) * fx) * (1 - fy) + (c + (d - c) * fx) * fy;
}

double worldgen::Fbm(double x, double y, std::int64_t seed, int octaves) {
	auto v = 0.0;
	auto amp = 1.0;
	auto total = 0.0;
	auto f = 1.0;
	for (auto o = 0; o < octaves; o++) {
		v += amp * ValueNoise(x * f, y * f, seed + o * 101);
		total += amp;
		amp *= 0.5;
		f *= 2.0;
	}
	return v / total;
}
