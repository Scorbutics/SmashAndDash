#pragma once
#include <SDL2/SDL_mixer.h>
#include "../Utils/Observer.h"
#include "../World/WorldEvent.h"
#include "SoundEvent.h"

namespace ska {
	class SoundManager : 
		public Observer<SoundEvent>,
		public Observer<ska::WorldEvent> {
	public:
		SoundManager(unsigned int channels);
		void play(Mix_Music* m_instance);
		bool handleSoundEvent(SoundEvent& se);
		bool handleWorldEvent(ska::WorldEvent& we);
		virtual ~SoundManager() = default;

	private:
		
	};

}