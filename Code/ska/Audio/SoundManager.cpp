#include <iostream>
#include <SDL2/SDL_mixer.h>
#include "SoundManager.h"

ska::SoundManager::SoundManager(unsigned int channels) : 
	Observer<SoundEvent>(std::bind(&SoundManager::handleEvent, this, std::placeholders::_1)) {
	if(Mix_AllocateChannels(channels) != channels) {
		std::cerr << "SoundManager error : " << Mix_GetError() << std::endl;
	}
}

void ska::SoundManager::play(Mix_Music* m_instance) {
	if(Mix_PlayMusic(m_instance, -1) == -1) {
		std::cerr << "SoundManager error : " << Mix_GetError() << std::endl;
	}
}

bool ska::SoundManager::handleEvent(SoundEvent& se) {
	auto music = se.getMusic();
	if (se.getEventType() == ska::SoundEventType::PLAY_MUSIC) {
		music->play(*this);
	}
	return false;
}
