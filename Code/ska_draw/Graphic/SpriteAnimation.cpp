#include "SpriteAnimation.h"
#include "Utils/SpritePath.h"



ska::SpriteAnimation::SpriteAnimation(int type, int id, Point<int> pos, unsigned int framesNumber, unsigned int delay, int alpha) : m_anim(delay, framesNumber, false),
m_sprite(SpritePath::getInstance().getPath(type, id), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, alpha)
{
	m_type = type;
	m_id = id;
	m_pos = pos;


	m_offset.x = 0;
	m_offset.y = 0;
	m_offset.w = static_cast<Uint16>(m_sprite.getWidth()/m_anim.getNumberFrames());
	m_offset.h = static_cast<Uint16>(m_sprite.getHeight());

//	ska::Rectangle& oRel = WGameCore::getInstance().getORel();

	/*if (m_pos.x == -1)
		m_pos.x = WGameCore::getInstance().getWidth() / 2 - m_offset.w / 2 - oRel.x;

	if (m_pos.y == -1)
		m_pos.y = WGameCore::getInstance().getHeight() / 2 - m_offset.h / 2 - oRel.y;*/

	m_anim.setOffsetAndFrameSize(m_offset);

}

ska::SpriteAnimation::SpriteAnimation(int type, int id, unsigned int framesNumber, unsigned int delay, int alpha) : m_anim(delay, framesNumber, false)
{
	m_type = type;
	m_id = id;
	m_pos.x = 0;
	m_pos.y = 0;

	m_sprite.load(SpritePath::getInstance().getPath(m_type, m_id), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, alpha);
	m_offset.x = 0;
	m_offset.y = 0;
	m_offset.w = static_cast<Uint16>(m_sprite.getWidth()/m_anim.getNumberFrames());
	m_offset.h = static_cast<Uint16>(m_sprite.getHeight());

	m_anim.setOffsetAndFrameSize(m_offset);

}


void ska::SpriteAnimation::refresh() {
	m_offset = m_anim.getRectOfCurrentFrame();
}


void ska::SpriteAnimation::setPos(Point<int> pos)
{
    m_pos = pos;
}

ska::Animation* ska::SpriteAnimation::getAnimation()
{
	return &m_anim;
}

void ska::SpriteAnimation::setID(int id)
{
    m_id = id;
	m_sprite.load(SpritePath::getInstance().getPath(m_type, m_id), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
	m_offset.x = 0;
	m_offset.y = 0;
	m_offset.w = static_cast<Uint16>(m_sprite.getWidth()/m_anim.getNumberFrames());
	m_offset.h = static_cast<Uint16>(m_sprite.getHeight());

	m_anim.setOffsetAndFrameSize(m_offset);
}

void ska::SpriteAnimation::setType(int spriteBankType)
{
    m_type = spriteBankType;
	m_sprite.load(SpritePath::getInstance().getPath(m_type, m_id), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
	m_offset.x = 0;
	m_offset.y = 0;
	m_offset.w = static_cast<Uint16>(m_sprite.getWidth()/m_anim.getNumberFrames());
	m_offset.h = static_cast<Uint16>(m_sprite.getHeight());

	m_anim.setOffsetAndFrameSize(m_offset);
}

unsigned int ska::SpriteAnimation::getHeight()
{
    return m_offset.h;
}

unsigned int ska::SpriteAnimation::getWidth()
{
    return m_offset.w;
}

void ska::SpriteAnimation::display() const
{
	/*WGameCore& wScreen = WGameCore::getInstance();*/
	const Point<int>& pos = m_pos;
    /*pos.x += wScreen.getORel().x;
    pos.y += wScreen.getORel().y;*/

	m_sprite.render(pos.x, pos.y, &m_offset);
}
