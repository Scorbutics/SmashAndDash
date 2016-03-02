#include "GameCore.h"
#include "../Data/Savegame.h"

ska::GameCore::GameCore(Savegame* saveGame) : m_scripts(*saveGame)
{
}


ska::GameCore::~GameCore()
{
}
