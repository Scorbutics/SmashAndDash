#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleSounds.h"

BattleSounds::BattleSounds() {
	const char* files[] = { "cast", "hit", "strong_hit", "guard", "status", "dash", "ko" };
	for (std::size_t index = 0; index < m_sounds.size(); index++) {
		m_sounds[index] = std::make_unique<ska::SDLSound>(std::string{ "./Sounds/battle_" } + files[index] + ".wav");
	}
}

void BattleSounds::play(BattleSound sound) const {
	const auto index = static_cast<std::size_t>(sound);
	if (index < m_sounds.size() && m_sounds[index] != nullptr) {
		m_sounds[index]->play();
	}
}
