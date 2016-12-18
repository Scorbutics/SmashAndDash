#pragma once
#include <fstream>

class Settings {
public:
    Settings(const std::string& fileName);

    void setSoundVolume(float sndVol);
    void setFog(bool b);
    void setWeather(bool b);
    void setGuiTransparency(bool b);

    void load();
    void save();

    const std::string& getFileName() const;
    bool getGuiTransparency() const;
    bool getFogActive() const;
    bool getWeatherActive() const;
    float getSoundVolume() const;
    //int* getParticles();

    ~Settings() = default;

private:
    const std::string m_fileName;
    bool m_fogActive;
	bool m_weatherActive;
	bool m_guiTransparency;
    float m_soundVol;
    //int m_particles;
};

