#ifndef DEF_TEXTURE
#define DEF_TEXTURE

#include <map>
#include <memory>
#include "../Utils\ResourceTemplate.h"
#include "SDLTexture.h"
#include "TextureData.h"
#include "../Utils/SkaConstants.h"
#include "Rectangle.h"
#include "Color.h"

namespace ska {
	class Window;
	class Texture : public ResourceTemplate<ska::SDLTexture, ska::TextureData> {

	public:
		Texture(std::string id, int r = DEFAULT_T_RED, int g = DEFAULT_T_GREEN, int b = DEFAULT_T_BLUE, int a = -1);
		Texture(std::string id, ska::Color finalColor);
		Texture();
		void loadFromText(unsigned int fontSize, std::string text, ska::Color c);
		void load(std::string id, int r = DEFAULT_T_RED, int g = DEFAULT_T_GREEN, int b = DEFAULT_T_BLUE, int a = -1);
		static void setDefaultWindow(ska::Window* w);
		static void freeAll();
		virtual ~Texture();

		void setColor(Uint8 red, Uint8 green, Uint8 blue);
		void setBlendMode(SDL_BlendMode blending);
		void operator=(const Texture& text);
		void setAlpha(Uint8 alpha);
		int render(int x, int y, const Rectangle* clip = NULL) const;

		const unsigned int getWidth() const;
		const unsigned int getHeight() const;

	private:
		static void checkWindow();
		static ska::Window* m_window;
	};
	typedef std::unique_ptr<Texture> TexturePtr;
}




#endif