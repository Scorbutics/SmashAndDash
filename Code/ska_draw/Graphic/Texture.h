#pragma once

#include <memory>
#include "Utils/ResourceTemplate.h"
#include "Draw/SDLTexture.h"
#include "Draw/TextureData.h"
#include "Utils/SkaConstants.h"
#include "Rectangle.h"
#include "Draw/Color.h"

namespace ska {
	class SDLRenderer;
	class Texture : public ResourceTemplate<SDLTexture, TextureData> {

	public:
		explicit Texture(const std::string& id, int r = DEFAULT_T_RED, int g = DEFAULT_T_GREEN, int b = DEFAULT_T_BLUE, int a = -1);
		Texture();
		void loadFromText(unsigned int fontSize, std::string text, Color c);
		void load(const std::string& id, int r = DEFAULT_T_RED, int g = DEFAULT_T_GREEN, int b = DEFAULT_T_BLUE, int a = -1);
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
		static void checkRenderer();
		static SDLRenderer* m_renderer;
	};
	typedef std::unique_ptr<Texture> TexturePtr;
}



