#include <iostream>
#include <fstream>
#include <string>
#include <SDL2/SDL.h>

#include "../../Utils\IDs.h"
#include "Inventory_Area.h"
#include "../../Gameplay\WGameCore.h"
#include "MouseCursor.h"
#include "../../Utils/ChargementImages.h"

using namespace std;

Inventory_Area::Inventory_Area(DialogMenu* parent, Inventory* inv, ska::Rectangle relativePos) : DynamicWindowArea(parent)
{
    m_type = BUTTON_INVENTORY_AREA;
    m_inv = inv;
    m_active = parent->isVisible();
    m_relativePos.x = relativePos.x;
    m_relativePos.y = relativePos.y;
    m_relativePos.w = relativePos.w;
    m_relativePos.h = relativePos.h;
    m_lastObjectPos.x = 0;
	m_lastObjectPos.y = 0;
	m_lastObjectPos.w = 0;
	m_lastObjectPos.h = 0;

}


Inventory* Inventory_Area::getInventory()
{
    return m_inv;
}

Object* Inventory_Area::getObjectAtPos(ska::Rectangle relativePos)
{
    unsigned int i = this->getIndexFromPos(relativePos);
    if(i < m_inv->getSize())
        return m_inv->getObjectFromIndex(i);
    return NULL;
}

unsigned int Inventory_Area::getAmountAtPos(ska::Rectangle relativePos)
{
    unsigned int i = this->getIndexFromPos(relativePos);
    if(i < m_inv->getSize())
        return m_inv->getAmountFromIndex(i);
    return 0;
}

unsigned int Inventory_Area::getIndexFromPos(ska::Rectangle relativePos)
{
    return (unsigned int) (relativePos.x/m_inv->getSquareSprite()->getWidth() + (relativePos.y/m_inv->getSquareSprite()->getHeight())*m_relativePos.w);
}

void Inventory_Area::useObjectAtPos(ska::Rectangle objectPos)
{
    Object* object = this->getObjectAtPos(objectPos);
    if(object != NULL)
        m_inv->use(object->getID(), 1);
}

void Inventory_Area::useObjectAtPos(ska::Rectangle objectPos, int i)
{
    Object* object = this->getObjectAtPos(objectPos);
    if(object != NULL)
        m_inv->use(object->getID(), 1, i);
}

void Inventory_Area::display()
{

    if(!m_parent->isVisible())
        return;
    else
        m_active = true;

	ska::Rectangle buf = m_relativePos;
    buf.x += (m_parent->getPos())->x;
    buf.y += (m_parent->getPos())->y;

    if(m_inv != NULL)
        m_inv->display(buf);

}

void Inventory_Area::refresh()
{
	ska::Rectangle buf = m_relativePos;
	buf.x += (m_parent->getPos())->x;
	buf.y += (m_parent->getPos())->y;

	WGameCore& wScreen = WGameCore::getInstance();
	MouseInput* in = wScreen.getInputListener().getMouseInput();
	MouseCursor& mouseCur = wScreen.getMouseCursor();
	ska::Rectangle mousePos = in->getMousePos();

	if(in->mouseClick(SDL_BUTTON_LEFT) && IsPositionInBox(&mousePos, &buf) && !IsPositionInBox(&mousePos, wScreen.getGUI().getClickMenu()->getPos()))
	{
		ska::Rectangle invAreaAbsolutePos = this->getAbsolutePos();
		wScreen.getGUI().getClickMenu()->hide(true);
		m_curObjectPos = mousePos;
		m_curObjectPos.x -= invAreaAbsolutePos.x;
		m_curObjectPos.y -= invAreaAbsolutePos.y;
		if(this->getObjectAtPos(m_curObjectPos) != NULL && mouseCur.getObject() == NULL)
		{
			unique_ptr<Object> objBuf = unique_ptr<Object>(new Object(getObjectAtPos(m_curObjectPos)->getID()));
			mouseCur.setObject(&(*objBuf), this->getAmountAtPos(m_curObjectPos));
			m_inv->remove(mouseCur.getObject()->getID(), mouseCur.getObjectAmount());
			m_lastObjectPos = m_curObjectPos;
		}
		else if(mouseCur.getObject() != NULL)
		{
			m_inv->add(mouseCur.getObject()->getID(), mouseCur.getObjectAmount());
			if(this->getObjectAtPos(m_curObjectPos) != NULL)
			{
				int index = this->getIndexFromPos(m_curObjectPos), indexLast = this->getIndexFromPos(m_lastObjectPos);
				m_inv->swap(index, indexLast);
			}

			mouseCur.removeObject();
		}

		in->setMouseLastState(SDL_BUTTON_LEFT, 0);
		in->setMouseState(SDL_BUTTON_LEFT, 0);

	}
}

Inventory_Area::~Inventory_Area()
{
}
