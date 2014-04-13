#ifndef DEF_SDLTEXTURE
#define DEF_SDLTEXTURE

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>


class Texture;

class SDLTexture
{
	friend class Texture;

	public:
		SDLTexture();
		void load(std::string id, int r = -1, int g = -1, int b = -1, int a = -1);
		void loadFromText(TTF_Font* font, std::string text, SDL_Color c);
		void free();
		~SDLTexture();
		
		

	private:
	int m_r, m_g, m_b;

	//La texture wrapp�e
	SDL_Texture* m_texture;

	//Largeur de la texture
	unsigned int m_w;

	//Hauteur de la texture
	unsigned int m_h;

	Uint8 m_alpha;

	//Nom du fichier qui est charg� lors d'un .load
	std::string m_fileName;
};
#endif