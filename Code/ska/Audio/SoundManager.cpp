#include <iostream>
#include <SDL2/SDL_mixer.h>
#include "SoundManager.h"

ska::SoundManager::SoundManager(unsigned int channels) : 
	Observer<SoundEvent>(std::bind(&SoundManager::handleSoundEvent, this, std::placeholders::_1)),
	Observer<ska::WorldEvent>(std::bind(&SoundManager::handleWorldEvent, this, std::placeholders::_1)) {
	if(Mix_AllocateChannels(channels) != channels) {
		std::cerr << "SoundManager error : " << Mix_GetError() << std::endl;
	}
}

void ska::SoundManager::play(Mix_Music* m_instance) {
	if(Mix_PlayMusic(m_instance, -1) == -1) {
		std::cerr << "SoundManager error : " << Mix_GetError() << std::endl;
	}
}

bool ska::SoundManager::handleSoundEvent(SoundEvent& se) {
	auto music = se.getMusic();
	if (se.getEventType() == ska::SoundEventType::PLAY_MUSIC) {
		music->play(*this);
	}
	return false;
}

bool ska::SoundManager::handleWorldEvent(ska::WorldEvent& we) {
	auto music = we.getBgm();
	if (we.getEventType() == ska::WorldEventType::WORLD_CREATE || 
		we.getEventType() == ska::WorldEventType::WORLD_CHANGE) {
		music->play(*this);
	}
	return false;
}
