#include <fstream>

#include "Settings.h"
#include "../../ska/Inputs/Readers/IniReader.h"
#include "../Weather.h"

Settings::Settings(PokemonGameEventDispatcher& ged, const std::string& file) : 
	m_fileName(file),
	m_ged(ged) {
	load();
}

void Settings::setSoundVolume(float sndVol) {
    m_soundVol = sndVol*100;
	sendEvent(MUSIC_VOLUME);
}

void Settings::setFog(bool b) {
    m_fogActive = b;
	sendEvent(FOG);
}

void Settings::load() {
    ska::IniReader reader(m_fileName);
	setSoundVolume(reader.get<float>("Settings sound_volume")/100);
	setFog(reader.get<bool>("Settings fog_active"));
	setWeather(reader.get<bool>("Settings weather_active"));
	setGuiTransparency(reader.get<bool>("Settings gui_transparency"));
	//m_particles = reader.get<int>("Settings particles_active");
}

void Settings::save() {
	ska::IniReader reader(m_fileName);
	reader.set("Settings sound_volume", m_soundVol);
	reader.set("Settings fog_active", m_fogActive);
	reader.set("Settings weather_active", m_weatherActive);
	reader.set("Settings gui_transparency", m_guiTransparency);
	//reader.set("Settings particles_active", m_particles);
	reader.save(m_fileName);
}

void Settings::setWeather(bool b) {
    m_weatherActive = b;
	sendEvent(WEATHER);
}

void Settings::setGuiTransparency(bool b) {
    m_guiTransparency = b;
	sendEvent(GUI_TRANSPARENCY);
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

float Settings::getSoundVolume() const {
    return m_soundVol;
}

void Settings::sendEvent(SettingsChangeEventType scet) const {
	SettingsChangeEvent sce(scet, *this);
	m_ged.ska::Observable<SettingsChangeEvent>::notifyObservers(sce);
}

// int* Settings::getParticles() const {
//     return &m_particles;
// }

