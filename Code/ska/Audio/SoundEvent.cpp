#include "SoundEvent.h"

ska::SoundEvent::SoundEvent(Music* music, SoundEventType set) : 
	m_music(music), 
	m_eventType(set) {

}

ska::Music* ska::SoundEvent::getMusic() {
	return m_music;
}

const ska::SoundEventType& ska::SoundEvent::getEventType() const {
	return m_eventType;
}
