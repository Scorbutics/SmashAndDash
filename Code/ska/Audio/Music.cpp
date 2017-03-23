#include "SoundRenderer.h"
#include "Music.h"

ska::Music::Music(const std::string& musicName) {
	if (!musicName.empty()) {
		loadFromKey(musicName);
	}
}

void ska::Music::setVolume(float vol) const{
	m_value->setVolume(vol);
}

void ska::Music::play(SoundRenderer& sndMgr) const{
	sndMgr.play(m_value->m_instance);
	if (static_cast<int>(m_value->m_volume) != -1) {
		sndMgr.setMusicVolume(m_value->m_volume);
	}
}

ska::Music::~Music() {
	free();
}
