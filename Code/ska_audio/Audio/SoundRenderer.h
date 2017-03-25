#pragma once
#include <SDL_mixer.h>
#include "Utils/Observer.h"
#include "Data/Events/WorldEvent.h"
#include "Data/Events/SoundEvent.h"

namespace ska {
	class SoundRenderer :
		public Observer<SoundEvent>,
		public Observer<WorldEvent> {
	public:
		SoundRenderer(unsigned int channels);
		void play(Mix_Music* m_instance);
		void setMusicVolume(float volPcts) const;
		bool handleSoundEvent(SoundEvent& se);
		bool handleWorldEvent(WorldEvent& we);
		virtual ~SoundRenderer() = default;

	private:
		Mix_Music* m_currentPlayed;
	};

}
