#include "Block.h"
#include "Gameplay\WGameCore.h"

using namespace std;

Block::Block(SDL_Rect corr, unsigned int idSprite, unsigned int properties, bool auto_animation, int col):m_anim(375, 4, true, 0, 0, TAILLEBLOC, TAILLEBLOC)
{
	m_properties = properties;

	m_auto_animation = auto_animation;

    m_collision = col;

	m_idSprite = idSprite;

	m_spritePosInChipset.x = idSprite%corr.w * TAILLEBLOC;
	m_spritePosInChipset.y = idSprite/corr.w * TAILLEBLOC;
	m_spritePosInChipset.w = TAILLEBLOC;
	m_spritePosInChipset.h = TAILLEBLOC;

	

}

Block::Block(SDL_Rect corr, SDL_Rect posChipset, unsigned int properties, bool auto_animation, int col):m_anim(375, 4, true, 0, 0, TAILLEBLOC, TAILLEBLOC)
{
	m_properties = properties;

	m_auto_animation = auto_animation;

    m_collision = col;

	m_idSprite = posChipset.x + posChipset.y * corr.w;

	m_spritePosInChipset.x = posChipset.x*TAILLEBLOC;
	m_spritePosInChipset.y = posChipset.y*TAILLEBLOC;
	m_spritePosInChipset.w = TAILLEBLOC;
	m_spritePosInChipset.h = TAILLEBLOC;

}

unsigned int Block::getProperties()
{
	return m_properties;
}

int Block::getCollision()
{
    return m_collision;
}

unsigned int Block::getID()
{
    return m_idSprite;
}

void Block::setSpriteFrame(unsigned int x)
{
	m_anim.setNumberFrames(x);
	m_anim.stop(true);
}

void Block::refresh(SDL_Rect pos, const SDL_Rect* rectAnim)
{
	SDL_Rect buf = m_spritePosInChipset, bufRectAnim;
	WGameCore& wScreen = WGameCore::getInstance();
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

	wScreen.getWorld().getChipset()->render(pos.x, pos.y, &buf);

}

Block::~Block()
{
}
