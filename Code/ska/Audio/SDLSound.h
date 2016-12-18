#pragma once
#include <string>
#include <SDL2/SDL_mixer.h>

namespace ska {
	class SDLSound {
	public:
		SDLSound(const std::string& soundName);
		virtual ~SDLSound();

	private:
		Mix_Chunk* m_instance;
	};
}