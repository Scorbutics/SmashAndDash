#pragma once

#include <SDL2\SDL.h>
#include "../Utils/NumberUtils.h"

namespace ska {
	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		Color(uint8_t pr,
		uint8_t pg,
		uint8_t pb,
		uint8_t pa) {
			r = pr;
			g = pg;
			b = pb;
			a = pa;
		}

		Color() {
			r = g = b = a = 0;
		}

		Color(const Color& c) {
			*this = c;
		}
			
		Color(const SDL_Color& c) {
			*this = c;
		}

		bool operator==(const Color& c) const {
			return r == c.r
				&& g == c.g
				&& b == c.b
				&& a == c.a;
		}

		void operator=(const Color& c) {
			r = c.r;
			g = c.g;
			b = c.b;
			a = c.a;
		}

		void operator=(const SDL_Color& c) {
			r = c.r;
			g = c.g;
			b = c.b;
			a = c.a;
		}

		SDL_Color toNative() {
			return SDL_Color{ r, g, b, a };
		}
	};

}

namespace std {
	template <>
	struct hash<ska::Color> {
		std::size_t operator()(const ska::Color& k) const {
			using std::size_t;
			using std::hash;

			std::size_t hashSeed = k.r;
			int g = (int)k.g;
			int b = (int)k.b;
			int a = (int)k.a;
			ska::NumberUtils::hashCombine<int>(hashSeed, g);
			ska::NumberUtils::hashCombine<int>(hashSeed, b);
			ska::NumberUtils::hashCombine<int>(hashSeed, a);
			return hashSeed;
		}
	};
}

