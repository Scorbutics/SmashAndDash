#include "SDLMusic.h"

ska::SDLMusic::SDLMusic(const std::string& musicPath) : m_volume(-1) {
	m_instance = Mix_LoadMUS(musicPath.c_str());
}

void ska::SDLMusic::setVolume(float vol) {
	m_volume = vol;
}

ska::SDLMusic::~SDLMusic() {
	Mix_FreeMusic(m_instance);
}
