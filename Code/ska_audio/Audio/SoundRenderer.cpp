#include <iostream>
#include <SDL_mixer.h>
#include "Music.h"
#include "SoundRenderer.h"
#include "Logging/Logger.h"

ska::SoundRenderer::SoundRenderer(unsigned int channels) :
	ska::Observer<SoundEvent>(bind(&SoundRenderer::handleSoundEvent, this, std::placeholders::_1)),
	ska::Observer<WorldEvent>(bind(&SoundRenderer::handleWorldEvent, this, std::placeholders::_1)),
	m_currentPlayed(nullptr) {

	if(Mix_AllocateChannels(channels) != static_cast<int>(channels)) {
		SKA_LOG_ERROR("SoundManager error : ", Mix_GetError());
	}
}

void ska::SoundRenderer::play(Mix_Music* m_instance) {
	Mix_PauseMusic();
	if(Mix_PlayMusic(m_instance, -1) == -1) {
		SKA_LOG_ERROR("SoundManager error : ", Mix_GetError());
		m_currentPlayed = nullptr;
	} else {
		m_currentPlayed = m_instance;
	}
}

void ska::SoundRenderer::setMusicVolume(float volPcts) const{
	if (m_currentPlayed != nullptr) {
		Mix_VolumeMusic(static_cast<int>((volPcts * MIX_MAX_VOLUME) / 100));
	}
}

bool ska::SoundRenderer::handleSoundEvent(SoundEvent& se) {
	auto music = se.getMusic();
	if (se.getEventType() == PLAY_MUSIC) {
		music->play(*this);
	}
	return false;
}

bool ska::SoundRenderer::handleWorldEvent(WorldEvent& we) {
	auto music = we.getBgm();
	if (we.getEventType() == WORLD_CREATE ||
		we.getEventType() == WORLD_CHANGE) {
		music->play(*this);
	}
	return false;
}
