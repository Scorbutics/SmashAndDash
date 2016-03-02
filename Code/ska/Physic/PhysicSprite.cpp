#include "PhysicSprite.h"
#include "../Graphic\SpritePath.h"
#include "../Graphic/Rectangle.h"
using namespace std;

ska::PhysicSprite::PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, ska::Rectangle r, int offset0, int offset1, int offset2, int offset3) :
	PhysicObject(id, entityNumber, weight, frictionCoeff, speedLimit, r, offset0, offset1, offset2, offset3), m_anim(150, 3, false),
	m_shadow(SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, SPRITE_ID_SHADOW), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128),
	m_sprite(SpritePath::getInstance().getPath(SPRITEBANK_PHYSIC, id), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE)
{

	m_visible = true;
	ska::Rectangle actualOffsetAnim;
	actualOffsetAnim.x = 0;
	actualOffsetAnim.y = 0;
	actualOffsetAnim.w = static_cast<Uint16>(m_sprite.getWidth() / 3);
	actualOffsetAnim.h = static_cast<Uint16>(m_sprite.getHeight());
	m_anim.setOffsetAndFrameSize(actualOffsetAnim);
	m_rect.w = actualOffsetAnim.w;
	m_rect.h = actualOffsetAnim.h;
	
	
}

ska::PhysicSprite::PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, ska::Rectangle r, int offset[4]) :
PhysicObject(id, entityNumber, weight, frictionCoeff, speedLimit, r, offset), m_anim(150, 3, false),
m_shadow(SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, SPRITE_ID_SHADOW), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128),
m_sprite(SpritePath::getInstance().getPath(SPRITEBANK_PHYSIC, id), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE)
{

	m_visible = true;
	ska::Rectangle actualOffsetAnim;
	actualOffsetAnim.x = 0;
	actualOffsetAnim.y = 0;
	actualOffsetAnim.w = static_cast<Uint16>(m_sprite.getWidth() / 3);
	actualOffsetAnim.h = static_cast<Uint16>(m_sprite.getHeight());
	m_anim.setOffsetAndFrameSize(actualOffsetAnim);
	m_rect.w = actualOffsetAnim.w;
	m_rect.h = actualOffsetAnim.h;
}

ska::PhysicSprite::PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, int x, int y, unsigned int w, unsigned int h, int offset0, int offset1, int offset2, int offset3) :
PhysicObject(id, entityNumber, weight, frictionCoeff, speedLimit, x, y, w, h, offset0, offset1, offset2, offset3), m_anim(150, 3, false),
m_shadow(SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, SPRITE_ID_SHADOW), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128),
m_sprite(SpritePath::getInstance().getPath(SPRITEBANK_PHYSIC, id), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE)
{

	m_visible = true;
	ska::Rectangle actualOffsetAnim;
	actualOffsetAnim.x = 0;
	actualOffsetAnim.y = 0;
	actualOffsetAnim.w = static_cast<Uint16>(m_sprite.getWidth() / 3);
	actualOffsetAnim.h = static_cast<Uint16>(m_sprite.getHeight());
	m_anim.setOffsetAndFrameSize(actualOffsetAnim);
	m_rect.w = actualOffsetAnim.w;
	m_rect.h = actualOffsetAnim.h;
}

void ska::PhysicSprite::displayShadow()
{
	ska::Rectangle anim;
	//WGameCore& wScreen = WGameCore::getInstance();
	

	anim.x = anim.y = 0;
	anim.w = m_shadow.getWidth()/2;
	anim.h = m_shadow.getHeight();

	m_shadow.render(getHitbox().x /*+ wScreen.getORel().x*/, m_rect.y + m_rect.h /*+ wScreen.getORel().y*/ - anim.h, &anim);
}

void ska::PhysicSprite::display()
{
	ska::Rectangle buf, actualOffsetAnim;
	/*WGameCore& wScreen = WGameCore::getInstance();*/

	if(!m_visible)
		return;

	buf = m_rect;
	/*buf.x += wScreen.getORel().x;
	buf.y += wScreen.getORel().y;*/

	actualOffsetAnim = m_anim.getRectOfCurrentFrame();
	this->displayShadow();
	m_sprite.render(buf.x, buf.y-m_jumpHeight, &actualOffsetAnim);
	
}


ska::Texture* ska::PhysicSprite::getSprite()
{
	return &m_sprite;
}

void ska::PhysicSprite::teleport(int x, int y)
{
	PhysicObject::teleport(x, y);
	adaptPositionToBlock();
}

void ska::PhysicSprite::show(bool x)
{
	m_visible = x;
}

bool ska::PhysicSprite::isVisible()
{
	return m_visible;
}

ska::Animation* ska::PhysicSprite::getAnimation()
{
	return &m_anim;
}

void ska::PhysicSprite::setID(int id)
{
	m_id = id;
	m_sprite.load(SpritePath::getInstance().getPath(SPRITEBANK_PHYSIC, id), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
}

void ska::PhysicSprite::setOffsetAndFrameSize(ska::Rectangle sprite)
{
	m_anim.setOffsetAndFrameSize(sprite);
	m_rect.w = sprite.w;
	m_rect.h = sprite.h;
}

ska::Rectangle ska::PhysicSprite::getOffsetAndFrameSize()
{
	return m_anim.getOffsetAndFrameSize();
}

int ska::PhysicSprite::getWidth()
{
	return m_anim.getOffsetAndFrameSize().w;
}

int ska::PhysicSprite::getHeight()
{
	return m_anim.getOffsetAndFrameSize().h;
}


//Sert à centrer la hitbox du personnage sur un bloc
void ska::PhysicSprite::adaptPositionToBlock()
{
	//WGameCore& wScreen = WGameCore::getInstance();

	ska::Rectangle bufLastRect = m_rect;
	/*m_rect.x = (((this->getHitboxCenterPos().x) / TAILLEBLOC)) *TAILLEBLOC;
	m_rect.y = (((this->getHitboxCenterPos().y) / TAILLEBLOC)) *TAILLEBLOC;
	m_rect.x -= m_offset[3];
	m_rect.y -= m_offset[2];
	
	
	if (collisionWorld() || !collisionNPC().empty())
		m_rect = bufLastRect;*/

	/*
	m_rect.x = ((this->getHitboxCenterPos().x/TAILLEBLOC)) *TAILLEBLOC;
	m_rect.y = ((this->getHitboxCenterPos().y/TAILLEBLOC)) *TAILLEBLOC;
	m_rect.x -= m_offset[3];
	m_rect.y -= m_offset[2];*/
}

ska::PhysicSprite::~PhysicSprite(){}

