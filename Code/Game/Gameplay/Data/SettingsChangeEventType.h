#pragma once

enum class SettingsChangeEventType {
	MUSIC_VOLUME = 0x0001,
	WEATHER = 0x0010,
	FOG = 0x0100,
	GUI_TRANSPARENCY = 0x1000,
	ALL = 0x1111
};
