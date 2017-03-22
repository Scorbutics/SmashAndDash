#pragma once
#include <string>
#include <SDL_mixer.h>

namespace ska {
	class SDLMusic {
		friend class Music;
	public:
		SDLMusic(const std::string& musicPath);
		void setVolume(float vol);
		virtual ~SDLMusic();

	private:
		Mix_Music* m_instance;
		float m_volume;
	};
}
