#pragma once

#include "SettingsChangeEvent.h"
#include "../PokemonGameEventDispatcher.h"
#include "../PokemonGameEventDispatcherDeclaration.h"

class Settings {
public:
    Settings(PokemonGameEventDispatcher& ged, const std::string& fileName);

    void setSoundVolume(float sndVol);
    void setFog(bool b);
    void setWeather(bool b);
    void setGuiTransparency(bool b);

	void operator=(const Settings&) = delete;

    void save();

    const std::string& getFileName() const;
    bool getGuiTransparency() const;
    bool getFogActive() const;
    bool getWeatherActive() const;
    float getSoundVolume() const;
    //int* getParticles();

    ~Settings() = default;

private:
	void load();
	void sendEvent(SettingsChangeEventType scet) const;

    const std::string m_fileName;
    bool m_fogActive;
	bool m_weatherActive;
	bool m_guiTransparency;
    float m_soundVol;
    //int m_particles;
	PokemonGameEventDispatcher& m_ged;
};

