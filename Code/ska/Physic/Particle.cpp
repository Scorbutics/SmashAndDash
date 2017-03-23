#include "Particle.h"
#include "../Graphic/SpritePath.h"
//#include "../Gameplay\WGameCore.h"

ska::Particle::Particle(int idSprite, unsigned int weight, Rectangle pos, double lifetime, double splashTime, bool loop, bool relative) :
m_sprite(SpritePath::getInstance().getPath(SPRITEBANK_PARTICLE, idSprite), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE),
m_anim(300, 2, false) {
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

ska::Rectangle ska::Particle::getPos()
{
    return m_pos;
}

void ska::Particle::launch(Point<int> origin, float angle, unsigned int power)
{
    this->active();
	m_fixedOrigin = origin;
	m_pos.x = origin.x;
	m_pos.y = origin.y;
    m_angle = angle;
    m_power = power;
    m_state = PARTICLE_STATE_LAUNCHED;
    //m_sprite = NULL;
}

ska::Point<int> ska::Particle::getOrigin() const
{
	return m_fixedOrigin;
}

unsigned int ska::Particle::getElapsedTime()
{
    return (unsigned int)m_t;
}

void ska::Particle::setElapsedTime(unsigned int t)
{
    m_t = t;
}


void ska::Particle::active()
{
    m_active = true;
    m_t = 0;
   // m_splashTime = 0;
}

void ska::Particle::destroy()
{
    m_active = false;
}

void ska::Particle::toSkillParticle()
{
    m_skill = true;
	m_sprite.load(SpritePath::getInstance().getPath(SPRITEBANK_SKILL, m_idSprite), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
	m_spriteSize.w = m_sprite.getWidth()/2;
	m_spriteSize.h = m_sprite.getHeight()/2;
	m_anim.setOffsetAndFrameSize(m_spriteSize);
}

int ska::Particle::getState()
{
    return m_state;
}

void ska::Particle::display()
{
	//WGameCore& wScreen = WGameCore::getInstance();

	/* Et encore une machine d'état un peu dégueu... */
    if(m_t - (m_lifetime + m_splashTime) >= 0)
    {
        m_state = PARTICLE_STATE_END;
        m_active = false;
    }
    else if(m_t - m_lifetime >= 0)
        m_state = PARTICLE_STATE_SPLASH;


	Rectangle animRect = { 0, 0, m_spriteSize.w, m_spriteSize.h }, relativePos = m_pos;
    /*relativePos.x += wScreen.getORel().x;
    relativePos.y += wScreen.getORel().y;*/


    animRect = m_anim.getRectOfCurrentFrame();
    if(m_state == PARTICLE_STATE_SPLASH)
        animRect.y += animRect.h;

	m_sprite.render(relativePos.x, relativePos.y, &animRect);

}

void ska::Particle::setLoop(bool loop)
{
	m_loop = loop;
}

bool ska::Particle::isActive()
{
    return m_active;
}

void ska::Particle::setNoise(int x)
{
    m_noise = x;
}

bool ska::Particle::isRelative()
{
    return m_relative;
}

bool ska::Particle::loop()
{
    return m_loop;
}

float ska::Particle::getAngle()
{
    return m_angle;
}

unsigned int ska::Particle::getPower()
{
    return m_power;
}

void ska::Particle::setPos(int x, int y)
{
    m_pos.x = x;
    m_pos.y = y;
}

std::vector<ska::Rectangle> ska::Particle::collisionNPC()
{
	//WGameCore& wScreen = WGameCore::getInstance();

	/*Rectangle rect = m_pos;
    rect.w = m_spriteSize.w;
    rect.h = m_spriteSize.h;*/

	return std::vector<Rectangle>();//wScreen.detectEntity(rect);
}

ska::Particle::~Particle()
{

}
