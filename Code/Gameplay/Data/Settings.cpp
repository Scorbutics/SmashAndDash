#include <fstream>

#include "Settings.h"
#include "../../ska/Inputs/Readers/IniReader.h"
#include "../Weather.h"

Settings::Settings(const std::string& file) : 
	m_fileName(file) {
    load();
}

void Settings::setSoundVolume(float sndVol) {
    m_soundVol = sndVol*100;
}

void Settings::setFog(bool b) {
    m_fogActive = b;
}

void Settings::load() {
    ska::IniReader reader(m_fileName);
	m_soundVol = reader.get<int>("Settings sound_volume");
	m_fogActive = reader.get<int>("Settings fog_active");
	m_weatherActive = reader.get<int>("Settings weather_active");
	m_guiTransparency = reader.get<int>("Settings gui_transparency");
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
}

void Settings::setGuiTransparency(bool b) {
    m_guiTransparency = b;
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

// int* Settings::getParticles() const {
//     return &m_particles;
// }

