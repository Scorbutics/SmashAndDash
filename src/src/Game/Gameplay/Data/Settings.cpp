#include "Game/__internalConfig/LoggerConfig.h"
#include <algorithm>
#include <fstream>

#include "Settings.h"
#include "SavePaths.h"
#include "Base/IO/Readers/IniReader.h"
#include "../Weather.h"

namespace {
	std::string PlayerSettingsFile() {
		return SaveDirectory() + "settings.ini";
	}

	float Percent(float volume) {
		return std::clamp(volume * 100.F, 0.F, 100.F);
	}
}

Settings::Settings(PokemonGameEventDispatcher& ged, const std::string& file) :
	m_fileName(file), m_fogActive(false), m_weatherActive(false), m_guiTransparency(false), m_masterVol(100), m_musicVol(30), m_soundVol(60),
	m_ged(ged){}

void Settings::setMasterVolume(float volume) {
	m_masterVol = Percent(volume);
	changed(SettingsChangeEventType::MUSIC_VOLUME);
}

void Settings::setMusicVolume(float volume) {
	m_musicVol = Percent(volume);
	changed(SettingsChangeEventType::MUSIC_VOLUME);
}

void Settings::setSoundVolume(float volume) {
	m_soundVol = Percent(volume);
	changed(SettingsChangeEventType::MUSIC_VOLUME);
}

void Settings::setFog(bool b) {
    m_fogActive = b;
	changed(SettingsChangeEventType::FOG);
}

void Settings::setWeather(bool b) {
    m_weatherActive = b;
	changed(SettingsChangeEventType::WEATHER);
}

void Settings::setGuiTransparency(bool b) {
    m_guiTransparency = b;
	changed(SettingsChangeEventType::GUI_TRANSPARENCY);
}

void Settings::load() {
	m_loading = true;
	const auto read = [this](const ska::IniReader& reader) {
		const auto volume = [&reader](const std::string& key, float current) { return reader.exists("Settings " + key) ? reader.get<float>("Settings " + key) / 100.F : current / 100.F; };
		setMasterVolume(volume("master_volume", m_masterVol));
		setMusicVolume(volume("music_volume", m_musicVol));
		setSoundVolume(volume("sound_volume", m_soundVol));
		if (reader.exists("Settings fog_active")) {
			setFog(reader.get<bool>("Settings fog_active"));
		}
		if (reader.exists("Settings weather_active")) {
			setWeather(reader.get<bool>("Settings weather_active"));
		}
		if (reader.exists("Settings gui_transparency")) {
			setGuiTransparency(reader.get<bool>("Settings gui_transparency"));
		}
	};
	// The defaults of the game, then the choices of the player
	read(ska::IniReader{ m_fileName });
	auto player = ska::IniReader{};
	if (player.load(PlayerSettingsFile())) {
		read(player);
	}
	m_loading = false;
}

void Settings::save() const {
	auto file = std::ofstream{ PlayerSettingsFile(), std::ios::trunc };
	if (!file) {
		SLOG(ska::LogLevel::Warn) << "The settings cannot be written in " << PlayerSettingsFile();
		return;
	}
	file << "[Settings]\n"
		<< "master_volume=" << static_cast<int>(m_masterVol) << "\n"
		<< "music_volume=" << static_cast<int>(m_musicVol) << "\n"
		<< "sound_volume=" << static_cast<int>(m_soundVol) << "\n"
		<< "fog_active=" << (m_fogActive ? "true" : "false") << "\n"
		<< "weather_active=" << (m_weatherActive ? "true" : "false") << "\n"
		<< "gui_transparency=" << (m_guiTransparency ? "true" : "false") << "\n";
}

bool Settings::getGuiTransparency() const {
    return m_guiTransparency;
}

bool Settings::getFogActive() const {
    return m_fogActive;
}

bool Settings::getWeatherActive() const {
    return m_weatherActive;
}

void Settings::changed(SettingsChangeEventType scet) const {
	sendEvent(scet);
	if (!m_loading) {
		save();
	}
}

void Settings::sendEvent(SettingsChangeEventType scet) const {
	SettingsChangeEvent sce(scet, *this);
	m_ged.ska::Observable<SettingsChangeEvent>::notifyObservers(sce);
}
