#pragma once

namespace ska {
	class Music;
	enum SoundEventType {
		PLAY_MUSIC,
		PLAY_SOUND,
		STOP_MUSIC,
		STOP_SOUND
	};

	class SoundEvent {
	public:
		SoundEvent(Music* music, SoundEventType set);
		virtual ~SoundEvent() = default;
		Music* getMusic();
		const SoundEventType& getEventType() const;

	private:
		Music* m_music;
		const SoundEventType m_eventType;

	};
}
