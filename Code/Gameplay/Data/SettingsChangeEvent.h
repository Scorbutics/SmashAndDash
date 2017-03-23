#pragma once
#include "SettingsChangeEventType.h"

class Settings;


struct SettingsChangeEvent {
	SettingsChangeEvent(SettingsChangeEventType scet, const Settings& s) :
		settings(s), eventType(scet) {
	}

	void operator=(const SettingsChangeEvent&) = delete;

	const Settings& settings;
	const SettingsChangeEventType eventType;

	~SettingsChangeEvent() = default;

};
