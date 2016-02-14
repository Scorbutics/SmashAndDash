#include <iostream>
#include "SDLTexture.h"
#include "../Gameplay\WGameCore.h"
#include "Font.h"

using namespace std;

SDLTexture::SDLTexture()
{
}

SDLTexture::SDLTexture(TextureData p)
{
	std::pair<string, SDL_Color> key = p.getData();
	load(key.first, key.second.r, key.second.g, key.second.b, key.second.a);
}

void SDLTexture::load(std::string fileName, int r, int g, int b, int a)
{
	WGameCore& wScreen = WGameCore::getInstance();
	SDL_Surface *sprite;
	m_texture = NULL;
	m_fileName = fileName;
	m_r = r;
	m_g = g;
	m_b = b;
	m_w = 0;
	m_h = 0;

	m_w = m_h = 0;
	if (a > 0 && a < 255)
		m_alpha = a;
	else
		m_alpha = 255;



	sprite = IMG_Load(m_fileName.c_str());

	if (sprite == NULL)
		cerr << "Erreur lors du chargement de l'image \"" << m_fileName << "\" : " << SDL_GetError() << endl;
	else if (m_r >= 0 && m_g >= 0 && m_b >= 0)
		SDL_SetColorKey(sprite, SDL_TRUE, SDL_MapRGBA((sprite)->format, m_r, m_g, m_b, m_alpha));

	//255 : opaque, donc pas besoin d'appliquer de l'alpha
	//si négatif, cela veut dire qu'on ne veut pas d'alpha non plus
	if (m_alpha != 255)
		SDL_SetSurfaceAlphaMod(sprite, m_alpha);

	if (sprite == NULL)
		sprite = IMG_Load(NOSUCHFILE);
	if (sprite == NULL)
	{
		cerr << "Impossible de charger l'image \"" << NOSUCHFILE << "\" : " << SDL_GetError() << endl;
		exit(EXIT_FAILURE);
	}
	
	free();
	m_fileName = fileName;
	m_texture = SDL_CreateTextureFromSurface(wScreen.getRenderer(), sprite);
	m_w = sprite->w;
	m_h = sprite->h;

	SDL_FreeSurface(sprite);
}

void SDLTexture::loadFromText(unsigned int fontSize, string text, SDL_Color c)
{
	WGameCore& wScreen = WGameCore::getInstance();
	SDL_Surface* buffer;
	Font f(fontSize);

	buffer = TTF_RenderText_Solid(f.getInstance(), text.c_str(), c);

	if (buffer == NULL)
	{
		cerr << "Erreur lors de la création de l'image-texte : " << TTF_GetError() << endl;
		exit(EXIT_FAILURE);
	}

	free();
	m_fileName = text;
	m_texture = SDL_CreateTextureFromSurface(wScreen.getRenderer(), buffer);
	m_h = buffer->h;
	m_w = buffer->w;
	m_alpha = c.a;

	SDL_FreeSurface(buffer);
}

void SDLTexture::free()
{
	if (m_texture != NULL)
	{
		SDL_DestroyTexture(m_texture);
		m_texture = NULL;
	}
	m_fileName = NOSUCHFILE;
	m_h = m_w = 0;
	m_alpha = 255;
}

SDLTexture::~SDLTexture()
{
	free();
}
