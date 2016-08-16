#include <fstream>

#include "Settings.h"
#include "../../ska/Inputs\Readers\IniReader.h"
#include "../Weather.h"

Settings::Settings(std::string file)
{
    m_fileName = file;
    this->load();
}

void Settings::setSoundVolume(int sndVol)
{
    m_soundVol = sndVol;
}

void Settings::setFog(int b)
{
    m_fogActive = b;
}

void Settings::setFileName(std::string fileName)
{
    m_fileName = fileName;
}

void Settings::update() {
	//WGameCore& core = WGameCore::getInstance();
	/*Weather* fog = core.getWorld().getFog();
	if (fog != NULL) {
		fog->hide(!m_fogActive);
	}
	Weather* weather = core.getWorld().getWeather();
	if (weather != NULL) {
		weather->hide(!m_weatherActive);
	}
	core.getParticleManager().hide(!m_particles);*/
	//core.getWorld().setBgmVolume(m_soundVol);
}

void Settings::load()
{
    ska::IniReader reader(m_fileName);
	m_soundVol = reader.get<int>("Settings sound_volume");
	m_fogActive = reader.get<int>("Settings fog_active");
	m_weatherActive = reader.get<int>("Settings weather_active");
	m_guiTransparency = reader.get<int>("Settings gui_transparency");
	m_particles = reader.get<int>("Settings particles_active");
	update();
}

void Settings::save()
{
	ska::IniReader reader(m_fileName);
	reader.set("Settings sound_volume", m_soundVol);
	reader.set("Settings fog_active", m_fogActive);
	reader.set("Settings weather_active", m_weatherActive);
	reader.set("Settings gui_transparency", m_guiTransparency);
	reader.set("Settings particles_active", m_particles);
	reader.save(m_fileName);
}

std::string Settings::getFileName()
{
    return m_fileName;
}

void Settings::setWeather(int b)
{
    m_weatherActive = b;
}

void Settings::setGuiTransparency(int b)
{
    m_guiTransparency = b;
}

int* Settings::getGuiTransparency()
{
    return &m_guiTransparency;
}

int* Settings::getFogActive()
{
    return &m_fogActive;
}

int* Settings::getWeatherActive()
{
    return &m_weatherActive;
}

int* Settings::getSoundVolume()
{
    return &m_soundVol;
}

int* Settings::getParticles()
{
    return &m_particles;
}

Settings::~Settings()
{}

