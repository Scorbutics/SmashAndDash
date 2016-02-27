#include "Particle.h"
#include "../Graphic\SpritePath.h"
#include "../Gameplay\WGameCore.h"

Particle::Particle(int idSprite, unsigned int weight, SDL_Rect pos, double lifetime, double splashTime, bool loop, bool relative): m_anim(300, 2, false),
m_sprite(SpritePath::getInstance().getPath(SPRITEBANK_PARTICLE, idSprite), T_RED, T_GREEN, T_BLUE)
{
    m_slopeNoise = 0;
    m_skill = false;
    m_pos = pos;
	m_fixedOrigin = pos;
    m_relative = relative;
    m_weight = weight;
    m_type = PARTICLE;
    m_idSprite = idSprite;
    m_lifetime = lifetime;
    m_state = PARTICLE_STATE_CREATED;
    m_t = 0;
    m_splashTime = splashTime;
    m_active = true;
    m_loop = loop;
    m_spriteSize.x = 0;
    m_spriteSize.y = 0;
    m_noise = 0;

	
	m_spriteSize.w = m_sprite.getWidth()/2;
	m_spriteSize.h = m_sprite.getHeight()/2;
	m_anim.setOffsetAndFrameSize(m_spriteSize);

}

SDL_Rect Particle::getPos()
{
    return m_pos;
}

void Particle::launch(SDL_Rect origin, float angle, unsigned int power)
{
    this->active();
	m_fixedOrigin = origin;
	m_pos = origin;
    m_angle = angle;
    m_power = power;
    m_state = PARTICLE_STATE_LAUNCHED;
    //m_sprite = NULL;
}

SDL_Rect Particle::getOrigin() const
{
	return m_fixedOrigin;
}

unsigned int Particle::getElapsedTime()
{
    return (unsigned int)m_t;
}

void Particle::setElapsedTime(unsigned int t)
{
    m_t = t;
}


void Particle::active()
{
    m_active = true;
    m_t = 0;
   // m_splashTime = 0;
}

void Particle::destroy()
{
    m_active = false;
}

void Particle::toSkillParticle()
{
    m_skill = true;
	m_sprite.load(SpritePath::getInstance().getPath(SPRITEBANK_SKILL, m_idSprite), T_RED, T_GREEN, T_BLUE);
	m_spriteSize.w = m_sprite.getWidth()/2;
	m_spriteSize.h = m_sprite.getHeight()/2;
	m_anim.setOffsetAndFrameSize(m_spriteSize);
}

int Particle::getState()
{
    return m_state;
}

void Particle::display()
{
	WGameCore& wScreen = WGameCore::getInstance();
	
	/* Et encore une machine d'état un peu dégueu... */
    if(m_t - (m_lifetime + m_splashTime) >= 0)
    {
        m_state = PARTICLE_STATE_END;
        m_active = false;
    }
    else if(m_t - m_lifetime >= 0)
        m_state = PARTICLE_STATE_SPLASH;


    SDL_Rect animRect = {0, 0, m_spriteSize.w, m_spriteSize.h}, relativePos = m_pos;
    relativePos.x += wScreen.getORel().x;
    relativePos.y += wScreen.getORel().y;


    animRect = m_anim.getRectOfCurrentFrame();
    if(m_state == PARTICLE_STATE_SPLASH)
        animRect.y += animRect.h;

	m_sprite.render(relativePos.x, relativePos.y, &animRect);

}

void Particle::setLoop(bool loop)
{
	m_loop = loop;
}

bool Particle::isActive()
{
    return m_active;
}

void Particle::setNoise(int x)
{
    m_noise = x;
}

bool Particle::isRelative()
{
    return m_relative;
}

bool Particle::loop()
{
    return m_loop;
}

float Particle::getAngle()
{
    return m_angle;
}

unsigned int Particle::getPower()
{
    return m_power;
}

void Particle::setPos(int x, int y)
{
    m_pos.x = x;
    m_pos.y = y;
}

vector<SDL_Rect> Particle::collisionNPC()
{
	WGameCore& wScreen = WGameCore::getInstance();

    SDL_Rect rect = m_pos;
    rect.w = m_spriteSize.w;
    rect.h = m_spriteSize.h;

    return wScreen.detectEntity(rect);
}

Particle::~Particle()
{

}
