#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleSounds.h"

namespace {
	// In the order of BattleSound (the clash, the encounter, the bond : Sounds/CREDITS.txt)
	const char* Files[] = { "battle_cast.wav", "battle_hit.wav", "battle_strong_hit.wav", "battle_guard.wav", "battle_status.wav", "battle_dash.wav", "battle_ko.wav",
		"clash_impact.ogg", "clash_woosh.ogg", "clash_shatter.wav", "clash_versus.ogg", "clash_swoosh.wav", "clash_land.wav", "clash_round.wav", "clash_shine.ogg",
		"clash_attack.ogg", "clash_hit.wav", "clash_ko.ogg", "clash_cursor.wav", "clash_select.wav",
		"clash_ball_throw.wav", "clash_ball_in.wav", "clash_ball_drop.wav", "clash_ball_shake.wav", "clash_ball_caught.wav", "clash_ball_open.wav",
		"battle_encounter.wav", "bond_start.ogg", "bond_charge.ogg", "bond_perfect.ogg", "bond_release.ogg",
		"battle_stat_up.wav", "battle_stat_down.wav" };
	static_assert(sizeof(Files) / sizeof(Files[0]) == static_cast<std::size_t>(BattleSound::Count), "a file for each sound");
}

BattleSounds::BattleSounds() {
	for (std::size_t index = 0; index < m_sounds.size(); index++) {
		m_sounds[index] = std::make_unique<ska::SDLSound>(std::string{ "./Sounds/" } + Files[index]);
	}
}

void BattleSounds::play(BattleSound sound, float gain) const {
	const auto index = static_cast<std::size_t>(sound);
	if (index < m_sounds.size() && m_sounds[index] != nullptr) {
		SLOG(ska::LogLevel::Debug) << "Sound " << Files[index];
		m_sounds[index]->play(gain);
	}
}
