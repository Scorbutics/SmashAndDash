#pragma once
#include <array>
#include <memory>
#include "Audio/SDLSound.h"
#include "BattleStage.h"

// The sounds of the battles (Sounds/battle_*.wav), played over the music
class BattleSounds {
public:
	BattleSounds();
	// "gain" : its volume (0 .. 1), times the volume of the sounds
	void play(BattleSound sound, float gain = 1.F) const;

private:
	std::array<std::unique_ptr<ska::SDLSound>, static_cast<std::size_t>(BattleSound::Count)> m_sounds;
};
