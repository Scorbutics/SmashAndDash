#include <math.h>
//#include "../Gameplay\WGameCore.h"
#include "LinearParticle.h"
#include "../Utils/SkaConstants.h"

ska::LinearParticle::LinearParticle(int idSprite, Rectangle pos, double lifetime, double splashTime, bool loop, bool relative) :
	Particle(idSprite, 0, pos, lifetime, splashTime, loop, relative)
{
    m_type = PARTICLE_LINEAR;
    m_ay = m_by = 0;
    m_ax = m_bx = 0;
}

void ska::LinearParticle::refresh()
{
	//WGameCore& wScreen = WGameCore::getInstance();

	if(m_state == PARTICLE_STATE_LAUNCHED)
    {
        m_pos.x = (int)(m_ax * (m_t/10.) + (float) m_bx /*+ wScreen.getORel().x*/);
		m_pos.y = (int)(m_ay * (m_t / 10.) + (float)m_by /*+ wScreen.getORel().y*/);
    }
    else if (m_state == PARTICLE_STATE_SPLASH)
    {
		m_pos.x = (int)(m_ax * (m_lifetime / 10.) + (float)m_bx /*+ wScreen.getORel().x*/);
		m_pos.y = (int)(m_ay * (m_lifetime / 10.) + (float)m_by /*+ wScreen.getORel().y*/);
    }

    float angle;
    if(static_cast<int>(m_ax) != 0)
    {
        angle = atan(((float)m_ay)/m_ax);
        if(m_ax < 0)
            angle += (float)M_PI;
    }
    else
        angle = (float)M_PI/2;

    m_pos.x += (int)(m_noise*cos(angle + M_PI/2));
    m_pos.y += (int)(m_noise*sin(angle + M_PI/2));

    m_t++;


}


void ska::LinearParticle::addSlopeNoise(float x)
{
    const int to = 4;
    m_slopeNoise += (float)(exp(-m_t/to)*x);
	m_ay -= (float)(exp(-m_t / to)*x*cos(m_angle));
	m_ax += (float)(exp(-m_t / to)*x*sin(m_angle));
}

void ska::LinearParticle::resetSlopeNoise()
{
    m_ay += m_slopeNoise*cos(m_angle);
    m_ax -= m_slopeNoise*sin(m_angle);
    m_slopeNoise = 0;
}

void ska::LinearParticle::launch(Rectangle origin, float angle, unsigned int power)
{
	Particle::launch(origin, angle, power);
	m_ay = power*sin(angle);
    m_by = (float)origin.y;
    m_ax = power*cos(angle);
    m_bx = (float)origin.x;
}

ska::LinearParticle::~LinearParticle()
{
}
