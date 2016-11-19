#pragma once

#include <SDL2/SDL.h>
#include "Color.h"

namespace ska {
	class Font;

	class SDLSurface {
	public:
		SDLSurface();
		~SDLSurface();

		void load(const std::string& file);
		void load32(const std::string& file);
		void loadFromText(const ska::Font& font, const std::string& text, ska::Color c);

		ska::Color getPixel32Color(int x, int y) const;
		Uint32 getPixel32(int x, int y) const;
		const SDL_PixelFormat * getFormat() const;

		SDL_Surface* getInstance() const;

	private:
		void free();
		SDL_Surface* m_surface;
	};
}
