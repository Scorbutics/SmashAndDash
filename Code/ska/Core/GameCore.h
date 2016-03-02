#pragma once
#include "../Script/ScriptDispatcher.h"

namespace ska {
	class Savegame;
	class GameCore
	{
	public:
		GameCore(Savegame* saveGame);
		virtual ~GameCore();
	private:
		ScriptDispatcher m_scripts;
		Savegame* m_saveGame;
	};
}
