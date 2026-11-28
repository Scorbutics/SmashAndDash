#include "Game/__internalConfig/LoggerConfig.h"
#include "MapSounds.h"
#include <string>

namespace {
	// In the order of MapSound (Sounds/CREDITS.txt)
	const char* Files[] = { "map_run_step.wav", "map_run_step2.wav", "map_bump.wav", "map_jump.wav", "map_land.wav" };
	static_assert(sizeof(Files) / sizeof(Files[0]) == static_cast<std::size_t>(MapSound::Count), "a file for each sound");
}

void MapSounds::play(MapSound sound, float gain) {
	if (!m_loaded) {
		m_loaded = true;
		for (std::size_t index = 0; index < m_sounds.size(); index++) {
			m_sounds[index] = std::make_unique<ska::SDLSound>(std::string{ "./Sounds/" } + Files[index]);
		}
	}
	const auto index = static_cast<std::size_t>(sound);
	if (index < m_sounds.size() && m_sounds[index] != nullptr) {
		SLOG(ska::LogLevel::Debug) << "Sound " << Files[index];
		m_sounds[index]->play(gain);
	}
}
