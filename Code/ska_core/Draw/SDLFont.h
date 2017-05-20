#pragma once

#include <map>
#include <SDL_ttf.h>

namespace ska {
	class SDLFont {
	public:
		SDLFont();
		
		SDLFont(SDLFont&) = delete;
		SDLFont(SDLFont&&) = default;
		SDLFont& operator=(const SDLFont&) = delete;
		SDLFont& operator=(SDLFont&&) = default;

		explicit SDLFont(unsigned int fontSize);
		~SDLFont();

		TTF_Font* getInstance();

	private:
		void open(const std::string& fontFile, unsigned int fontSize);
		TTF_Font* m_fontC;


	};
}
