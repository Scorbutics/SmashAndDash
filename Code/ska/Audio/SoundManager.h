#pragma once
#include <SDL2/SDL_mixer.h>
#include "../Utils/Observer.h"
#include "SoundEvent.h"

namespace ska {
	class SoundManager : public Observer<SoundEvent> {
	public:
		SoundManager(unsigned int channels);
		void play(Mix_Music* m_instance);
		bool handleEvent(SoundEvent& se);
		virtual ~SoundManager() = default;

	private:
		
	};

}