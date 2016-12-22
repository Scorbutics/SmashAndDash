#pragma once
#include <SDL2/SDL_mixer.h>
#include "../Utils/Observer.h"
#include "../World/WorldEvent.h"
#include "SoundEvent.h"

namespace ska {
	class SoundRenderer : 
		public Observer<SoundEvent>,
		public Observer<ska::WorldEvent> {
	public:
		SoundRenderer(unsigned int channels);
		void play(Mix_Music* m_instance);
		void setMusicVolume(float volPcts);
		bool handleSoundEvent(SoundEvent& se);
		bool handleWorldEvent(ska::WorldEvent& we);
		virtual ~SoundRenderer() = default;

	private:
		Mix_Music* m_currentPlayed;
	};

}