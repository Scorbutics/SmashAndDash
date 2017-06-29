#include <iostream>
#include "SDLFont.h"
#include "../Utils/SkaConstants.h"
#include "../Logging/Logger.h"

ska::SDLFont::SDLFont() {
	m_fontC = nullptr;
	open(DEFAULT_FONT_FILE, 12);
}

ska::SDLFont::SDLFont(unsigned int fontSize) {
	m_fontC = nullptr;
	open(DEFAULT_FONT_FILE, fontSize);
}

void ska::SDLFont::open(const std::string& fontFile, unsigned int fontSize) {
	TTF_CloseFont(m_fontC);
	m_fontC = TTF_OpenFont(fontFile.c_str(), fontSize);
	if (m_fontC == nullptr) {
		SKA_LOG_ERROR("Erreur lors de l'ouverture de la police : ", TTF_GetError());
	}
}



TTF_Font* ska::SDLFont::getInstance() {
	return m_fontC;
}

ska::SDLFont::~SDLFont() {
	TTF_CloseFont(m_fontC);
}
