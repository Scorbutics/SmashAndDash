#include "PhysicSprite.h"
#include "WGameCore.h"
#include "Texture.h"
#include "SpritePath.h"

using namespace std;

PhysicSprite::PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, SDL_Rect r, int offset0, int offset1, int offset2, int offset3) :
	PhysicObject(id, entityNumber, weight, frictionCoeff, speedLimit, r, offset0, offset1, offset2, offset3), m_anim(150, 3, false),
	m_shadow(SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, SPRITE_ID_SHADOW), T_RED, T_GREEN, T_BLUE, 128),
	m_sprite(SpritePath::getInstance().getPath(SPRITEBANK_PHYSIC, id), T_RED, T_GREEN, T_BLUE)
{

	m_visible = true;
	SDL_Rect actualOffsetAnim;
	actualOffsetAnim.x = 0;
	actualOffsetAnim.y = 0;
	actualOffsetAnim.w = static_cast<Uint16>(m_sprite.getWidth() / 3);
	actualOffsetAnim.h = static_cast<Uint16>(m_sprite.getHeight());
	m_anim.setOffsetAndFrameSize(actualOffsetAnim);
	m_rect.w = actualOffsetAnim.w;
	m_rect.h = actualOffsetAnim.h;
	
	
}

PhysicSprite::PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, SDL_Rect r, int offset[4]) :
PhysicObject(id, entityNumber, weight, frictionCoeff, speedLimit, r, offset), m_anim(150, 3, false),
m_shadow(SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, SPRITE_ID_SHADOW), T_RED, T_GREEN, T_BLUE, 128),
m_sprite(SpritePath::getInstance().getPath(SPRITEBANK_PHYSIC, id), T_RED, T_GREEN, T_BLUE)
{

	m_visible = true;
	SDL_Rect actualOffsetAnim;
	actualOffsetAnim.x = 0;
	actualOffsetAnim.y = 0;
	actualOffsetAnim.w = static_cast<Uint16>(m_sprite.getWidth() / 3);
	actualOffsetAnim.h = static_cast<Uint16>(m_sprite.getHeight());
	m_anim.setOffsetAndFrameSize(actualOffsetAnim);
	m_rect.w = actualOffsetAnim.w;
	m_rect.h = actualOffsetAnim.h;
}

PhysicSprite::PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, int x, int y, unsigned int w, unsigned int h, int offset0, int offset1, int offset2, int offset3):
PhysicObject(id, entityNumber, weight, frictionCoeff, speedLimit, x, y, w, h, offset0, offset1, offset2, offset3), m_anim(150, 3, false),
m_shadow(SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, SPRITE_ID_SHADOW), T_RED, T_GREEN, T_BLUE, 128),
m_sprite(SpritePath::getInstance().getPath(SPRITEBANK_PHYSIC, id), T_RED, T_GREEN, T_BLUE)
{

	m_visible = true;
	SDL_Rect actualOffsetAnim;
	actualOffsetAnim.x = 0;
	actualOffsetAnim.y = 0;
	actualOffsetAnim.w = static_cast<Uint16>(m_sprite.getWidth() / 3);
	actualOffsetAnim.h = static_cast<Uint16>(m_sprite.getHeight());
	m_anim.setOffsetAndFrameSize(actualOffsetAnim);
	m_rect.w = actualOffsetAnim.w;
	m_rect.h = actualOffsetAnim.h;
}

void PhysicSprite::displayShadow()
{
	SDL_Rect anim;
	WGameCore& wScreen = WGameCore::getInstance();
	

	anim.x = anim.y = 0;
	anim.w = m_shadow.getWidth()/2;
	anim.h = m_shadow.getHeight();

	m_shadow.render(getHitbox().x + wScreen.getORel().x, m_rect.y + m_rect.h + wScreen.getORel().y - anim.h, &anim);
}

void PhysicSprite::display()
{
	SDL_Rect buf, actualOffsetAnim;
	WGameCore& wScreen = WGameCore::getInstance();

	if(!m_visible)
		return;

	buf = m_rect;
	buf.x += wScreen.getORel().x;
	buf.y += wScreen.getORel().y;

	actualOffsetAnim = m_anim.getRectOfCurrentFrame();
	this->displayShadow();
	m_sprite.render(buf.x, buf.y-m_jumpHeight, &actualOffsetAnim);
	
}


Texture* PhysicSprite::getSprite()
{
	return &m_sprite;
}

void PhysicSprite::teleport(int x, int y)
{
	PhysicObject::teleport(x, y);
	adaptPositionToBlock();
}

void PhysicSprite::show(bool x)
{
	m_visible = x;
}

bool PhysicSprite::isVisible()
{
	return m_visible;
}

Animation* PhysicSprite::getAnimation()
{
	return &m_anim;
}

void PhysicSprite::setID(int id)
{
	m_id = id;
	m_sprite.load(SpritePath::getInstance().getPath(SPRITEBANK_PHYSIC, id), T_RED, T_GREEN, T_BLUE);
}

void PhysicSprite::setOffsetAndFrameSize(SDL_Rect sprite)
{
	m_anim.setOffsetAndFrameSize(sprite);
	m_rect.w = sprite.w;
	m_rect.h = sprite.h;
}

SDL_Rect PhysicSprite::getOffsetAndFrameSize()
{
	return m_anim.getOffsetAndFrameSize();
}

int PhysicSprite::getWidth()
{
	return m_anim.getOffsetAndFrameSize().w;
}

int PhysicSprite::getHeight()
{
	return m_anim.getOffsetAndFrameSize().h;
}


//Sert à centrer la hitbox du personnage sur un bloc
void PhysicSprite::adaptPositionToBlock()
{
	WGameCore& wScreen = WGameCore::getInstance();

	SDL_Rect bufLastRect = m_rect;
	m_rect.x = (((this->getHitboxCenterPos().x) / TAILLEBLOC)) *TAILLEBLOC;
	m_rect.y = (((this->getHitboxCenterPos().y) / TAILLEBLOC)) *TAILLEBLOC;
	m_rect.x -= m_offset[3];
	m_rect.y -= m_offset[2];
	
	
	if (collisionWorld() || !collisionNPC().empty())
		m_rect = bufLastRect;

	/*
	m_rect.x = ((this->getHitboxCenterPos().x/TAILLEBLOC)) *TAILLEBLOC;
	m_rect.y = ((this->getHitboxCenterPos().y/TAILLEBLOC)) *TAILLEBLOC;
	m_rect.x -= m_offset[3];
	m_rect.y -= m_offset[2];*/
}

PhysicSprite::~PhysicSprite(){}

