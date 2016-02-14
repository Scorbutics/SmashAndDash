#include <fstream>

#include "Settings.h"
#include "../../Inputs\Readers\IniReader.h"


using namespace std;
Settings::Settings(string file)
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

void Settings::setFileName(string fileName)
{
    m_fileName = fileName;
}

void Settings::load()
{
    IniReader reader(m_fileName);
	m_soundVol = reader.getInt("Settings sound_volume");
	m_fogActive = reader.getInt("Settings fog_active");
	m_weatherActive = reader.getInt("Settings weather_active");
	m_guiTransparency = reader.getInt("Settings gui_transparency");
	m_particles = reader.getInt("Settings particles_active");

}

void Settings::save()
{
	IniReader reader(m_fileName);
	reader.set("Settings sound_volume", m_soundVol);
	reader.set("Settings fog_active", m_fogActive);
	reader.set("Settings weather_active", m_weatherActive);
	reader.set("Settings gui_transparency", m_guiTransparency);
	reader.set("Settings particles_active", m_particles);
	reader.save(m_fileName);
}

string Settings::getFileName()
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

