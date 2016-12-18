#pragma once

#include "../Utils/ResourceTemplate.h"
#include "SDLMusic.h"

namespace ska {
	class SoundManager;

	class Music : public ResourceTemplate<SDLMusic, std::string> {
	public:
		Music(const std::string& musicName);
		void setVolume(float vol);
		void play(SoundManager& sndMgr);
		virtual ~Music();
	};
}