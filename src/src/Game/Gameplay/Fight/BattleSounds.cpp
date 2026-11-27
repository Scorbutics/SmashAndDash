#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleSounds.h"

BattleSounds::BattleSounds() {
	// In the order of BattleSound (the clash : Sounds/CREDITS.txt)
	const char* files[] = { "battle_cast.wav", "battle_hit.wav", "battle_strong_hit.wav", "battle_guard.wav", "battle_status.wav", "battle_dash.wav", "battle_ko.wav",
		"clash_impact.ogg", "clash_woosh.ogg", "clash_shatter.wav", "clash_versus.ogg", "clash_swoosh.wav", "clash_land.wav", "clash_round.wav", "clash_shine.ogg",
		"clash_attack.ogg", "clash_hit.wav", "clash_ko.ogg", "clash_cursor.wav", "clash_select.wav",
		"clash_ball_throw.wav", "clash_ball_in.wav", "clash_ball_drop.wav", "clash_ball_shake.wav", "clash_ball_caught.wav", "clash_ball_open.wav" };
	static_assert(sizeof(files) / sizeof(files[0]) == static_cast<std::size_t>(BattleSound::Count), "a file for each sound");
	for (std::size_t index = 0; index < m_sounds.size(); index++) {
		m_sounds[index] = std::make_unique<ska::SDLSound>(std::string{ "./Sounds/" } + files[index]);
	}
}

void BattleSounds::play(BattleSound sound) const {
	const auto index = static_cast<std::size_t>(sound);
	if (index < m_sounds.size() && m_sounds[index] != nullptr) {
		m_sounds[index]->play();
	}
}
