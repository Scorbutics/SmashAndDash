#ifndef DEF_PARTICLE
#define DEF_PARTICLE

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "Animation.h"
#include "Texture.h"

class Particle
{

    public:
    Particle(int idSprite, unsigned int weight, SDL_Rect pos, double lifetime, double splashTime, bool loop, bool relative);
    virtual void refresh(int wind) = 0;
    void display();
    virtual void launch(SDL_Rect origin, float angle, unsigned int power);
    virtual void addSlopeNoise(float x) = 0;
	virtual void resetSlopeNoise() = 0;
    SDL_Rect getOrigin();
    void active();
    void destroy();
    bool loop();
    unsigned int getElapsedTime();
    void setElapsedTime(unsigned int t);
    bool isActive();
    bool isRelative();
    unsigned int getPower();
    float getAngle();
    int getState();
    SDL_Rect getPos();
    std::vector<SDL_Rect> collisionNPC();
    void setPos(int x, int y);
    void toSkillParticle();
    void setNoise(int x);
    virtual ~Particle();
	void setLoop(bool loop);

    protected:
    Texture m_sprite;
    unsigned int m_weight, m_power, m_countWind;
    SDL_Rect m_pos, m_origin, m_spriteSize;
    Animation m_anim;
    float m_angle, m_slopeNoise;
    bool m_active, m_loop, m_relative, m_skill;
    int m_idSprite, m_type, m_state, m_noise;
    double m_t, m_lifetime, m_splashTime; //m_splashTime : temps pendant lequel la particule affiche son sprite de fin de vie (mettre à 0 s'il n'y a pas de sprite de fin de vie)

};

#endif
