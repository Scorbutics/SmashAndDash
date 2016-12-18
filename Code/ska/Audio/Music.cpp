#include "SoundManager.h"
#include "Music.h"

ska::Music::Music(const std::string& musicName) {
	if (!musicName.empty()) {
		ska::ResourceTemplate<SDLMusic, std::string>::load(musicName);
	}
}

void ska::Music::setVolume(float vol) {
	m_value->setVolume(vol);
}

void ska::Music::play(SoundManager& sndMgr) {
	sndMgr.play(m_value->m_instance);
}

ska::Music::~Music() {
	free();
}
