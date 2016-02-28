#include "Block.h"

using namespace std;

ska::Block::Block(const unsigned int blockSize, ska::Rectangle corr, unsigned int idSprite, unsigned int properties, bool auto_animation, int col) : m_anim(375, 4, true, 0, 0, blockSize, blockSize)
{
	m_properties = properties;

	m_auto_animation = auto_animation;

    m_collision = col;

	m_idSprite = idSprite;

	m_spritePosInChipset.x = idSprite%corr.w * blockSize;
	m_spritePosInChipset.y = idSprite / corr.w * blockSize;
	m_spritePosInChipset.w = blockSize;
	m_spritePosInChipset.h = blockSize;

}

ska::Block::Block(const unsigned int blockSize, ska::Rectangle corr, ska::Rectangle posChipset, unsigned int properties, bool auto_animation, int col) : m_anim(375, 4, true, 0, 0, blockSize, blockSize)
{
	m_properties = properties;

	m_auto_animation = auto_animation;

    m_collision = col;

	m_idSprite = posChipset.x + posChipset.y * corr.w;

	m_spritePosInChipset.x = posChipset.x*blockSize;
	m_spritePosInChipset.y = posChipset.y*blockSize;
	m_spritePosInChipset.w = blockSize;
	m_spritePosInChipset.h = blockSize;

}

unsigned int ska::Block::getProperties()
{
	return m_properties;
}

int ska::Block::getCollision()
{
    return m_collision;
}

unsigned int ska::Block::getID()
{
    return m_idSprite;
}

void ska::Block::setSpriteFrame(unsigned int x)
{
	m_anim.setNumberFrames(x);
	m_anim.stop(true);
}

void ska::Block::refresh(ska::Rectangle pos, const ska::Rectangle* rectAnim)
{
	ska::Rectangle buf = m_spritePosInChipset, bufRectAnim;
	//WGameCore& wScreen = WGameCore::getInstance();
	if(rectAnim != NULL && m_auto_animation)
	{
		bufRectAnim = *rectAnim;
		buf.x += bufRectAnim.x;
		buf.y += bufRectAnim.y;
	}
	else if(m_auto_animation)
	{
		bufRectAnim = m_anim.getRectOfCurrentFrame();
		buf.x += bufRectAnim.x;
		buf.y += bufRectAnim.y;

	}

	//wScreen.getWorld().getChipset()->render(pos.x, pos.y, &buf);

}

ska::Block::~Block()
{
}
