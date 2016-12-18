#pragma once
#include "../Audio/SoundManager.h"

namespace ska {
	class Savegame;
	class Game {
	public:
		Game();
		SoundManager& getSoundManager();
		virtual ~Game();

	private:
		SoundManager m_soundManager;
	};
}
