#include "SoundRenderer.h"
#include "Music.h"

ska::Music::Music(const std::string& musicName) {
	if (!musicName.empty()) {
		ska::ResourceTemplate<SDLMusic, std::string>::load(musicName);
	}
}

void ska::Music::setVolume(float vol) {
	m_value->setVolume(vol);
}

void ska::Music::play(SoundRenderer& sndMgr) {
	sndMgr.play(m_value->m_instance);
	if (m_value->m_volume != -1) {
		sndMgr.setMusicVolume(m_value->m_volume);
	}
}

ska::Music::~Music() {
	free();
}
