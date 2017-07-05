#pragma once

#include <SDL.h>
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
			r = g = b = 0;
			a = 255;
		}

		Color(const Color& c) {
			*this = c;
		}

		Color(Color&&) = default;

		explicit Color(const SDL_Color& c) {
			*this = c;
		}

		Color& operator= (Color&&) = default;

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

		template<class T>
		Color& operator*=(const T& factor) {
			r *= factor;
			g *= factor;
			b *= factor;
			a *= factor;
			return *this;
		}

		template<class T>
		Color operator*(const T& factor) {
			return Color(static_cast<uint8_t>(r * factor), 
						static_cast<uint8_t>(g * factor), 
						static_cast<uint8_t>(b * factor), 
						static_cast<uint8_t>(a * factor));
		}

		template<class T>
		Color operator+(const T& adder) {
			return Color(static_cast<uint8_t>(r + adder), 
						static_cast<uint8_t>(g + adder), 
						static_cast<uint8_t>(b + adder), 
						static_cast<uint8_t>(a + adder));
		}

		SDL_Color toNative() const {
			return SDL_Color{ r, g, b, a };
		}
	};

    template<>
    Color Color::operator+(const Color& adder);
}



namespace std {
	template <>
	struct hash<ska::Color> {
		size_t operator()(const ska::Color& k) const {
			using std::size_t;
			using std::hash;

			size_t hashSeed = k.r;
			const auto& g = static_cast<int>(k.g);
			const auto& b = static_cast<int>(k.b);
			const auto& a = static_cast<int>(k.a);
			ska::NumberUtils::hashCombine<int>(hashSeed, g);
			ska::NumberUtils::hashCombine<int>(hashSeed, b);
			ska::NumberUtils::hashCombine<int>(hashSeed, a);
			return hashSeed;
		}
	};
}

