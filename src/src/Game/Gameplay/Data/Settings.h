#pragma once
#include <string>
#include "SettingsChangeEvent.h"
#include "../PokemonGameEventDispatcher.h"

// The settings of the player : the volumes (of everything, of the music, of the sounds : percents), the weather, the fog.
// Their defaults are in gamesettings.ini, the choices of the player in the saves (<SaveDirectory>/settings.ini : the
// page keeps them on the web), written at each change.
class Settings {
public:
    Settings(PokemonGameEventDispatcher& ged, const std::string& fileName);

	// From 0 to 1 (the sliders of the settings)
	void setMasterVolume(float volume);
	void setMusicVolume(float volume);
	void setSoundVolume(float volume);
    void setFog(bool b);
    void setWeather(bool b);
    void setGuiTransparency(bool b);

	void operator=(const Settings&) = delete;

	void load();
    void save() const;

    bool getGuiTransparency() const;
    bool getFogActive() const;
    bool getWeatherActive() const;
	// Percents
	float getMasterVolume() const { return m_masterVol; }
	float getMusicVolume() const { return m_musicVol; }
	float getSoundVolume() const { return m_soundVol; }

    ~Settings() = default;

private:
	void sendEvent(SettingsChangeEventType scet) const;
	void changed(SettingsChangeEventType scet) const;

    const std::string m_fileName;
    bool m_fogActive;
	bool m_weatherActive;
	bool m_guiTransparency;
	float m_masterVol;
	float m_musicVol;
    float m_soundVol;
	// Loading : the values are not written back
	bool m_loading = false;
	PokemonGameEventDispatcher& m_ged;
};
