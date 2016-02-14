#include <iostream>
#include "SDLFont.h"
#include "../Utils\IDs.h"

SDLFont::SDLFont()
{
	m_fontC = NULL;
	open(12);
}

SDLFont::SDLFont(UInteger fontSize)
{
	m_fontC = NULL;
	open(fontSize.getData());
}

void SDLFont::open(unsigned int fontSize)
{
	TTF_CloseFont(m_fontC);
	m_fontC = TTF_OpenFont(FILE_FONT, fontSize);
	if (m_fontC == NULL)
		std::cerr << "Erreur lors de l'ouverture de la police : " << TTF_GetError() << std::endl;
}



TTF_Font* SDLFont::getInstance()
{
	return m_fontC;
}

SDLFont::~SDLFont()
{
	TTF_CloseFont(m_fontC);
}
