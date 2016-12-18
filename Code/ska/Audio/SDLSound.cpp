#include "SDLSound.h"

ska::SDLSound::SDLSound(const std::string& soundName) : m_instance(nullptr) {
	m_instance = Mix_LoadWAV(soundName.c_str());
}

ska::SDLSound::~SDLSound() {
	Mix_FreeChunk(m_instance);
}
