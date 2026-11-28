#pragma once
#include <array>
#include <memory>
#include "Audio/SDLSound.h"

// The sounds of the player on the map (Sounds/map_*.wav), played over the music : its steps when it runs, the bump of a
// wall it pushes against, its jumps (the jump key, a ledge) and their landing
enum class MapSound { RunStep, RunStepOther, Bump, Jump, Land, Count };

class MapSounds {
public:
	// "gain" : its volume (0 .. 1), times the volume of the sounds. The files are read at the first sound
	void play(MapSound sound, float gain = 1.F);

private:
	bool m_loaded = false;
	std::array<std::unique_ptr<ska::SDLSound>, static_cast<std::size_t>(MapSound::Count)> m_sounds;
};
