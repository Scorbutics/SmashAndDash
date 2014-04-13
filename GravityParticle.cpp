#include <math.h>
#include "WGameCore.h"
#include "GravityParticle.h"
#include "IDs.h"

GravityParticle::GravityParticle(int idSprite, SDL_Rect pos, double lifetime, double splashTime, float acceleration, bool loop, bool relative):
	Particle(idSprite, 0, pos, lifetime, splashTime, loop, relative)
{
    m_type = PARTICLE_GRAVITY;
    m_ay = m_by = m_cy = 0;
    m_ax = m_bx = 0;
    m_ay = acceleration;
    m_countWind = 0;
}

void GravityParticle::launch(SDL_Rect origin, float angle, unsigned int power)
{
    Particle::launch(origin, angle, power);
    m_by = power*sin(angle);
    m_cy = (float)origin.y;
    m_ax = power*cos(angle);
	m_bx = (float)origin.x;
}

void GravityParticle::addSlopeNoise(float x)
{
    m_slopeNoise += x;
    m_ax += x*sin(m_angle);
    m_by += x*cos(m_angle);
}

void GravityParticle::resetSlopeNoise()
{
    m_by -= m_slopeNoise*cos(m_angle);
    m_ax -= m_slopeNoise*sin(m_angle);
    m_slopeNoise = 0;
}

void GravityParticle::refresh(int offsetx)
{
    //if(m_t - (m_lifetime + m_splashTime) >= 0)
     //  return;

    if(m_state == PARTICLE_STATE_LAUNCHED)
    {
        m_pos.x = (int)(m_ax * (m_t/10.) + (float) m_bx  + offsetx);
        m_pos.y = (int)(m_ay * (m_t/10.) * (m_t/10.) + m_by * (m_t/10.) + (float) m_cy);
    }
    else if (m_state == PARTICLE_STATE_SPLASH)
    {
		m_pos.x = (int)(m_ax * (m_lifetime / 10.) + (float)m_bx);
		m_pos.y = (int)(m_ay * (m_lifetime / 10.) * (m_lifetime / 10.) + m_by * (m_lifetime / 10.) + (float)m_cy);
    }

    /*if(m_relative)
    {
        m_pos.x -= wm->getORel()->x;
        m_pos.y -= wm->getORel()->y;
    }*/
    m_t++;

}



GravityParticle::~GravityParticle(){
}
