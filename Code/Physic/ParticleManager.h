#ifndef DEF_PARTICLE_MANAGER
#define DEF_PARTICLE_MANAGER

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "Particle.h"
#include "../Graphic\Texture.h"

class ParticleManager
{

    public:
    ParticleManager();
    ~ParticleManager();

    void playCrumbling(int idSprite, float acceleration, float density, unsigned int duration);
    void playRain(int idSprite, float acceleration, float density, unsigned int duration);
    void playEffect(int idSprite, float acceleration, float density, unsigned int duration, SDL_Rect rect);
    void removeAll();
    void removeAll(unsigned int particleType);
    void stop();
    void stop(unsigned int particleType);
    void refresh();
    void refresh(unsigned int particleType);
    int collisionNPC(unsigned int particleType);
    void displayRainFog();
	void display(unsigned int particleType);

    protected:
    void remove(unsigned int particleType, unsigned int index);
    void launch(unsigned int particleType, float angle, unsigned int power);
    void addGravityParticle(unsigned int particleType, int idSprite, double lifetime, double splashTime, float acceleration, bool loop, bool relative);
    void addGravityParticle(unsigned int particleType, int idSprite, SDL_Rect pos, double lifetime, double splashTime, float acceleration, bool loop, bool relative);
    void addCircleParticle(int idSprite, unsigned int weight);
    void addLinearParticle(int idSprite, unsigned int weight);
	
    std::vector<std::vector<std::unique_ptr<Particle>>> m_particles;
    unsigned int m_wind, m_countWind;
    Texture m_rainWeather;
    double m_Twind;
    int m_sens;
    std::vector<SDL_Rect> m_origin;
    std::vector<bool> m_active;
    std::vector<unsigned int> m_duration, m_t0;

};

#endif
