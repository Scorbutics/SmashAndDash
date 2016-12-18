#include "SDLMusic.h"

ska::SDLMusic::SDLMusic(const std::string& musicPath) {
	Mix_PauseMusic();
	m_instance = Mix_LoadMUS(musicPath.c_str());
}

void ska::SDLMusic::setVolume(float vol) {
	Mix_VolumeMusic((MIX_MAX_VOLUME*vol)/100);
}

ska::SDLMusic::~SDLMusic() {
	Mix_FreeMusic(m_instance);
}
