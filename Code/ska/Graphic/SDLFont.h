#ifndef DEF_SDLFONT
#define DEF_SDLFONT

#include <map>
#include <memory>
#include <SDL2/SDL_ttf.h>
#include "../Utils/UInteger.h"

namespace ska {
	class SDLFont
	{
	public:
		SDLFont();
		SDLFont(UInteger fontSize);
		~SDLFont();

		TTF_Font* getInstance();

	private:
		void open(const std::string& fontFile, unsigned int fontSize);
		TTF_Font* m_fontC;


	};
}
#endif
