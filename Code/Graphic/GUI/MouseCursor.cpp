#include <string>
#include <vector>
#include <SDL2/SDL.h>

#include "MouseCursor.h"
#include "../../Gameplay/WGameCore.h"
#include "../SpritePath.h"
#include "../../Gameplay/Data/Statistics.h"
#include "../../Utils/StringUtils.h"

MouseCursor::MouseCursor() : m_aniCursor(3, 3, false), m_hintBox()
{
    m_hideC = m_hideH = true;
    m_delay = 0;
    m_time = 0;
    m_in = WGameCore::getInstance().getInputListener().getMouseInput();
    SDL_Rect buf = m_in->getMousePos();
    buf.w = 3*TAILLEBLOCFENETRE;
    buf.h = 2*TAILLEBLOCFENETRE;
    m_stockObject = NULL;
    m_stockPkmn = NULL;

    m_sprite.load("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"mousec.png", T_RED, T_GREEN, T_BLUE);
    m_hintBox = unique_ptr<DialogMenu>(new DialogMenu("hintbox", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"hintboxmenu.png", buf, 20));
    buf.w = m_sprite.getWidth()/3;
    buf.h = m_sprite.getHeight();
    m_aniCursor.setOffsetAndFrameSize(buf); //initialisation de l'animation
}

Animation* MouseCursor::getAnimation()
{
    return &m_aniCursor;
}

void MouseCursor::modifyHint(string hint)
{
    (*m_hintBox).modifyText(hint);
    (*m_hintBox).hide(false);
}

void MouseCursor::hideCursor(bool x)
{
    m_hideC = x;
}

void MouseCursor::hideHint(bool x)
{
    m_hideH = x;
}

void MouseCursor::setObjectAmount(unsigned int x)
{
    m_objectAmount = x;
}

bool MouseCursor::isActiveCursor()
{
    return !m_hideC;
}

DialogMenu* MouseCursor::getHintBox()
{
	if(m_hintBox != NULL)
		return &(*m_hintBox);
	else
		return NULL;
}

bool MouseCursor::isActiveHint(GUI *g)
{
    SDL_Rect mousePos = m_in->getMousePos();
    int ind = g->isPositionOnButton(&mousePos);
    if(ind != -1)
        m_hideH = false;
    else
        m_hideH = true;

    return !m_hideH;
}

void MouseCursor::setCursorPos(SDL_Rect pos)
{
    m_cursorPos = pos;
}

void MouseCursor::showCursorTime(unsigned int delay)
{
    m_time = SDL_GetTicks();
    m_delay = delay;
    m_hideC = false;
}

bool MouseCursor::isVisible() const {
	return !m_hideC;
}

void MouseCursor::display()
{
	if (m_hideC) {
		return;
	}

	WGameCore& wScreen = WGameCore::getInstance();

    if(SDL_GetTicks() - m_time < m_delay)
    {
        SDL_Rect buf = m_aniCursor.getRectOfCurrentFrame(), relativeMousePos = m_cursorPos;
        relativeMousePos.x += wScreen.getORel().x;
        relativeMousePos.y += wScreen.getORel().y;

		m_sprite.render(relativeMousePos.x, relativeMousePos.y, &buf);
    }



}

void MouseCursor::displaySelectedPokemon()
{
    SDL_Rect rectPkmnSprite, mousePos = m_in->getMousePos();
    if(m_stockPkmn != NULL)
    {
		Texture pkmn(SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, m_stockPkmn->getID()), T_RED, T_GREEN, T_BLUE);
        rectPkmnSprite.x = 0;
        rectPkmnSprite.y = m_stockPkmn->getHeight();
        rectPkmnSprite.w = m_stockPkmn->getWidth();
        rectPkmnSprite.h = m_stockPkmn->getHeight();
		pkmn.render(mousePos.x, mousePos.y, &rectPkmnSprite);
    }
}

void MouseCursor::displaySelectedObject()
{
    SDL_Rect mousePos = m_in->getMousePos();
    if(m_stockObject != NULL)
    {
        mousePos.x += 10;
        m_stockObject->setPos(mousePos);
        m_stockObject->display();
    }


}

void MouseCursor::displayHint()
{
	WGameCore& wScreen = WGameCore::getInstance();
    SDL_Rect mousePos = wScreen.getInputListener().getMouseInput()->getMousePos();
    mousePos.y -= m_hintBox->getPos()->w/2;
    m_hintBox->setPos(mousePos);
    m_hintBox->display();
}

Object* MouseCursor::getObject()
{
	if(m_stockObject != NULL)
		return &(*m_stockObject);
	else
		return NULL;
}

Character* MouseCursor::getPokemon()
{
	if(m_stockPkmn != NULL)
		return &(*m_stockPkmn);
	else
		return NULL;
}



void MouseCursor::setPokemon(Character* pkmn)
{
	m_stockPkmn = Character_ptr(new Character(pkmn->getID()));
	m_stockPkmn->setEntityNumber(pkmn->getEntityNumber());
	m_stockPkmn->setSpeedLimit(pkmn->getSpeedLimit());
	m_stockPkmn->setDirection(pkmn->getDirection());
	m_stockPkmn->setEntityNumber(pkmn->getEntityNumber());
	m_stockPkmn->teleport(pkmn->getPos().x, pkmn->getPos().y);
	m_stockPkmn->setHP(pkmn->getHp());
	m_stockPkmn->getStatistics()->setExperience(pkmn->getStatistics()->getExperience());

	if(pkmn->getPath()->isMotionless())
		m_stockPkmn->getPath()->setPathString(StringUtils::intToStr(P_FIXED));
	else if((pkmn->getPath()->isRandom()))
		m_stockPkmn->getPath()->setPathString(StringUtils::intToStr(P_RANDOM));
	else
		m_stockPkmn->getPath()->setPathString(pkmn->getPath()->getPathString());

	for(unsigned int j = 0; j < 4; j++)
		m_stockPkmn->setOffset(j, pkmn->getOffset(j));
}

void MouseCursor::removePokemon()
{
	Character_ptr buf = std::move(m_stockPkmn);
	m_stockPkmn = NULL;
}

void MouseCursor::removeObject()
{
	unique_ptr<Object> buf = std::move(m_stockObject);
	m_stockObject = NULL;
}

unsigned int MouseCursor::getObjectAmount()
{
    return m_objectAmount;
}


void MouseCursor::setObject(Object* object, unsigned int amount)
{
	m_stockObject = unique_ptr<Object>(new Object(*object));
	m_objectAmount = amount;
}

MouseCursor::~MouseCursor()
{
}
