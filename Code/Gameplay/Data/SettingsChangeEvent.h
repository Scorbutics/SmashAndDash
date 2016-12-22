#pragma once

class Settings;

enum SettingsChangeEventType {
	MUSIC_VOLUME = 0x0001,
	WEATHER = 0x0010,
	FOG = 0x0100,
	GUI_TRANSPARENCY = 0x1000,
	ALL = 0x1111
};
	
struct SettingsChangeEvent {
	SettingsChangeEvent(SettingsChangeEventType scet, const Settings& s) : 
		eventType(scet), 
		settings(s) {
	}

	const Settings& settings;
	const SettingsChangeEventType eventType;

	~SettingsChangeEvent() = default;

};