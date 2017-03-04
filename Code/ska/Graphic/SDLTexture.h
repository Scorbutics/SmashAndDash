#ifndef DEF_SDLTEXTURE
#define DEF_SDLTEXTURE

#include <string>
#include "Color.h"

namespace ska {
	class Texture;
	class TextureData;
	class Window;
	class SDLTexture {
		friend class Texture;

	public:
		SDLTexture();
		SDLTexture(TextureData& p);
		void load(const Window& w, const std::string& id, int r = -1, int g = -1, int b = -1, int a = -1);
		void loadFromText(const Window& w, unsigned int fontSize, const std::string& text, Color c);
		void free();
		~SDLTexture();

	private:
		Uint8 m_r, m_g, m_b;

		//La texture wrappée
		SDL_Texture* m_texture;

		//Largeur de la texture
		unsigned int m_w;

		//Hauteur de la texture
		unsigned int m_h;

		Uint8 m_alpha;

		//Nom du fichier qui est chargé lors d'un .load
		std::string m_fileName;
	};
}
#endif