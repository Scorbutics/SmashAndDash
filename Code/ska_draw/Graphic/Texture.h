#pragma once

#include <memory>
#include "Utils/ResourceTemplate.h"
#include "SDLTexture.h"
#include "TextureData.h"
#include "Utils/SkaConstants.h"
#include "Rectangle.h"
#include "Color.h"

namespace ska {
	class Window;
	class Texture : public ResourceTemplate<SDLTexture, TextureData> {

	public:
		explicit Texture(std::string id, int r = DEFAULT_T_RED, int g = DEFAULT_T_GREEN, int b = DEFAULT_T_BLUE, int a = -1);
		Texture();
		void loadFromText(unsigned int fontSize, std::string text, Color c);
		void load(std::string id, int r = DEFAULT_T_RED, int g = DEFAULT_T_GREEN, int b = DEFAULT_T_BLUE, int a = -1);
		static void setDefaultWindow(Window* w);
		static void freeAll();
		virtual ~Texture();

		void setColor(Uint8 red, Uint8 green, Uint8 blue) const;
		void setBlendMode(SDL_BlendMode blending) const;
		void operator=(const Texture& text);
		void setAlpha(Uint8 alpha) const;
		int render(int x, int y, const Rectangle* clip = nullptr) const;

		unsigned int getWidth() const;
		unsigned int getHeight() const;

	private:
		static void checkWindow();
		static Window* m_window;
	};
	typedef std::unique_ptr<Texture> TexturePtr;
}



