#include "SpriteAnimation.h"
#include "WGameCore.h"
#include "IDs.h"
#include "Texture.h"
#include "SpritePath.h"

using namespace std;

SpriteAnimation::SpriteAnimation(int type, int id, SDL_Rect pos, unsigned int framesNumber, unsigned int delay, int alpha): m_anim(delay, framesNumber, false),
m_sprite(SpritePath::getInstance().getPath(type, m_id), T_RED, T_GREEN, T_BLUE, alpha)
{
	m_type = type;
	m_id = id;
	m_pos = pos;

	m_offset.x = 0;
	m_offset.y = 0;
	m_offset.w = static_cast<Uint16>(m_sprite.getWidth()/m_anim.getNumberFrames());
	m_offset.h = static_cast<Uint16>(m_sprite.getHeight());

	m_anim.setOffsetAndFrameSize(m_offset);

}

SpriteAnimation::SpriteAnimation(int type, int id, unsigned int framesNumber, unsigned int delay, int alpha): m_anim(delay, framesNumber, false)
{
	m_type = type;
	m_id = id;
	m_pos.x = 0;
	m_pos.y = 0;
	m_pos.w = 0;
	m_pos.h = 0;

	m_sprite.load(SpritePath::getInstance().getPath(m_type, m_id), T_RED, T_GREEN, T_BLUE, alpha);
	m_offset.x = 0;
	m_offset.y = 0;
	m_offset.w = static_cast<Uint16>(m_sprite.getWidth()/m_anim.getNumberFrames());
	m_offset.h = static_cast<Uint16>(m_sprite.getHeight());

	m_anim.setOffsetAndFrameSize(m_offset);

}


void SpriteAnimation::refresh()
{
	m_offset = m_anim.getRectOfCurrentFrame();
	this->display();
}


void SpriteAnimation::setPos(SDL_Rect pos)
{
    m_pos = pos;
}

Animation* SpriteAnimation::getAnimation()
{
	return &m_anim;
}

void SpriteAnimation::setID(int id)
{
    m_id = id;
	m_sprite.load(SpritePath::getInstance().getPath(m_type, m_id), T_RED, T_GREEN, T_BLUE);
	m_offset.x = 0;
	m_offset.y = 0;
	m_offset.w = static_cast<Uint16>(m_sprite.getWidth()/m_anim.getNumberFrames());
	m_offset.h = static_cast<Uint16>(m_sprite.getHeight());

	m_anim.setOffsetAndFrameSize(m_offset);
}

void SpriteAnimation::setType(int spriteBankType)
{
    m_type = spriteBankType;
	m_sprite.load(SpritePath::getInstance().getPath(m_type, m_id), T_RED, T_GREEN, T_BLUE);
	m_offset.x = 0;
	m_offset.y = 0;
	m_offset.w = static_cast<Uint16>(m_sprite.getWidth()/m_anim.getNumberFrames());
	m_offset.h = static_cast<Uint16>(m_sprite.getHeight());

	m_anim.setOffsetAndFrameSize(m_offset);
}

unsigned int SpriteAnimation::getHeight()
{
    return m_offset.h;
}

unsigned int SpriteAnimation::getWidth()
{
    return m_offset.w;
}

void SpriteAnimation::display()
{
	WGameCore& wScreen = WGameCore::getInstance();
    SDL_Rect pos = m_pos;
    pos.x += wScreen.getORel().x;
    pos.y += wScreen.getORel().y;

	m_sprite.render(pos.x, pos.y, &m_offset);
}
