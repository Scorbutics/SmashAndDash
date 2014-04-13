#ifndef DEF_TEXTURE
#define DEF_TEXTURE

#include <map>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
//#include "Resource1.h"
#include "SDLTexture.h"
#include "IDs.h"


class Texture
{
	
	public:
		Texture(std::string id, int r = T_RED, int g = T_GREEN, int b = T_BLUE, int a = -1);
		Texture();
		void loadFromText(TTF_Font* font, std::string text, SDL_Color c);
		void load(std::string id, int r = T_RED, int g = T_GREEN, int b = T_BLUE, int a = -1);
		void free();
		static void freeAll();
		~Texture();
	
		void setColor( Uint8 red, Uint8 green, Uint8 blue );
		void setBlendMode( SDL_BlendMode blending );
		Texture clone();
		void setAlpha( Uint8 alpha );
		int render( int x, int y, SDL_Rect* clip = NULL );
		
		int getWidth() const;
		int getHeight() const;
		
	private:
		std::shared_ptr<SDLTexture> m_instance;
		std::string m_id;
		static std::map<std::string, std::weak_ptr<SDLTexture>> m_container;

		
};





#endif