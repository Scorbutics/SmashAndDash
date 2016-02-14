#ifndef DEF_MONDE
#define DEF_MONDE

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <fmodex/fmod.h>

#include "Block.h"

#include <memory>
#include "../Weather.h"

class ParticleManager;

class Layer
{
    public:
    Layer(std::string nomFichier, std::string chipsetName, int windowWidth, int windowHeight);
    ~Layer();

    void playBgm(bool x);
    bool isBgmPlaying();
    void changeLevel(std::string fname, std::string chipsetname);
	void reset(std::string file, std::string chipsetName);
	void display(SDL_Rect rectAnim);
	void printCollisionProfile();
    void clear();


    Block* getBlock(unsigned int i, unsigned int j);
    int getBlockCollision(const unsigned int i, const unsigned int j);
    std::string getWorldName();
    int getNbrBlocX();
    int getNbrBlocY();
    std::string getChipsetName();
    void getData();
    Weather* getWeather();
    Weather* getFog();
    void getFogFromData(std::string stringDataFile);
    void getWeatherFromData(std::string stringdataFile);
    void getBgmFromData(std::string stringDataFile);
	void getRainFromData(std::string stringDataFile);

    void setBgm(std::string bgm);
    void setBgmVolume(int volPercent);
    void setWind(int windDirection);

    private:

    std::string m_name, m_nomFichier, m_chipsetname;
    std::vector<std::vector<Block_ptr>> m_block;
    std::unique_ptr<Weather> m_temps, m_brouillard;
    int m_nbrBlocX, m_nbrBlocY, m_windowHeight, m_windowWidth, m_seed, m_type, m_windDirection;

};
#endif
