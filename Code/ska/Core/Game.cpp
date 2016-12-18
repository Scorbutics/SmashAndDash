#include "Game.h"
#include "../Data/Savegame.h"

ska::Game::Game() : 
	m_soundManager(10) {
}


ska::SoundManager& ska::Game::getSoundManager() {
	return m_soundManager;
}

ska::Game::~Game() {
}
