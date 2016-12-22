#include <iostream>
#include <SDL2/SDL_mixer.h>
#include "SoundRenderer.h"

ska::SoundRenderer::SoundRenderer(unsigned int channels) : 
	Observer<SoundEvent>(std::bind(&SoundRenderer::handleSoundEvent, this, std::placeholders::_1)),
	Observer<ska::WorldEvent>(std::bind(&SoundRenderer::handleWorldEvent, this, std::placeholders::_1)),
	m_currentPlayed(nullptr) {

	if(Mix_AllocateChannels(channels) != channels) {
		std::cerr << "SoundManager error : " << Mix_GetError() << std::endl;
	}
}

void ska::SoundRenderer::play(Mix_Music* m_instance) {
	Mix_PauseMusic();
	if(Mix_PlayMusic(m_instance, -1) == -1) {
		std::cerr << "SoundManager error : " << Mix_GetError() << std::endl;
		m_currentPlayed = nullptr;
	} else {
		m_currentPlayed = m_instance;
	}
}

void ska::SoundRenderer::setMusicVolume(float volPcts) {
	if (m_currentPlayed != nullptr) {
		Mix_VolumeMusic((volPcts * MIX_MAX_VOLUME)/100);
	}
}

bool ska::SoundRenderer::handleSoundEvent(SoundEvent& se) {
	auto music = se.getMusic();
	if (se.getEventType() == ska::SoundEventType::PLAY_MUSIC) {
		music->play(*this);
	}
	return false;
}

bool ska::SoundRenderer::handleWorldEvent(ska::WorldEvent& we) {
	auto music = we.getBgm();
	if (we.getEventType() == ska::WorldEventType::WORLD_CREATE || 
		we.getEventType() == ska::WorldEventType::WORLD_CHANGE) {
		music->play(*this);
	}
	return false;
}
