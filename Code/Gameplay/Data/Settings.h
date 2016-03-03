#ifndef DEF_SETTINGS
#define DEF_SETTINGS

#include <fstream>

class Settings
{
    public:
    Settings(std::string fileName);

    void setSoundVolume(int sndVol);
    void setFog(int b);
    void setWeather(int b);
    void setGuiTransparency(int b);
    void setFileName(std::string fileName);

	void update();
    void load();
    void save();

    std::string getFileName();
    int* getGuiTransparency();
    int* getFogActive();
    int* getWeatherActive();
    int* getSoundVolume();
    int* getParticles();

    ~Settings();

    private:
    std::string m_fileName;
    int m_fogActive;
    int m_weatherActive;
    int m_guiTransparency;
    int m_soundVol; //En pourcents (0 = sans son, 100 = volume maximal)
    int m_particles;
};
#endif
