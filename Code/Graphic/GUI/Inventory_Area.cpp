#include <iostream>
#include <fstream>
#include <string>

#include "../../Utils\IDs.h"
#include "Inventory_Area.h"
#include "../../Gameplay\WGameCore.h"
#include "MouseCursor.h"
#include "../../Utils/ChargementImages.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "DialogMenu.h"

#define SIZE_ICON_ITEM_INVENTORY (TAILLEBLOCFENETRE / 4)

Inventory_Area::Inventory_Area(DialogMenu* parent, Inventory* inv, ska::Point<int> relativePos) : DynamicWindowArea(parent)
{
    m_type = BUTTON_INVENTORY_AREA;
    m_inv = inv;
    m_active = parent->isVisible();
    m_relativePos.x = relativePos.x;
    m_relativePos.y = relativePos.y;
    m_lastObjectPos.x = 0;
	m_lastObjectPos.y = 0;

}


Inventory* Inventory_Area::getInventory()
{
    return m_inv;
}

Object* Inventory_Area::getObjectAtPos(ska::Point<int> relativePos)
{
    unsigned int i = this->getIndexFromPos(relativePos);
    if(i < m_inv->getSize())
        return m_inv->getObjectFromIndex(i);
    return NULL;
}

unsigned int Inventory_Area::getAmountAtPos(ska::Point<int> relativePos)
{
    unsigned int i = this->getIndexFromPos(relativePos);
    if(i < m_inv->getSize())
        return m_inv->getAmountFromIndex(i);
    return 0;
}

unsigned int Inventory_Area::getIndexFromPos(ska::Point<int> relativePos)
{
	return (unsigned int)(relativePos.x / m_inv->getSquareSprite()->getWidth() + (relativePos.y / m_inv->getSquareSprite()->getHeight())*SIZE_ICON_ITEM_INVENTORY);
}

ska::Rectangle Inventory_Area::getRect() const {
	return ska::Rectangle { m_relativePos.x, m_relativePos.y, SIZE_ICON_ITEM_INVENTORY, SIZE_ICON_ITEM_INVENTORY };
}

void Inventory_Area::useObjectAtPos(ska::Point<int> objectPos)
{
    Object* object = this->getObjectAtPos(objectPos);
    if(object != NULL)
        m_inv->use(object->getID(), 1);
}

void Inventory_Area::useObjectAtPos(ska::Point<int> objectPos, int i)
{
    Object* object = this->getObjectAtPos(objectPos);
    if(object != NULL)
        m_inv->use(object->getID(), 1, i);
}

void Inventory_Area::display()
{

    if(!m_parent->isVisible())
        return;

    m_active = true;

	ska::Rectangle buf; 
	buf.x = m_relativePos.x;
	buf.y = m_relativePos.y;
	buf.x += (m_parent->getRect()).x;
	buf.y += (m_parent->getRect()).y;
	buf.w = SIZE_ICON_ITEM_INVENTORY;
	buf.h = SIZE_ICON_ITEM_INVENTORY;

    if(m_inv != NULL)
        m_inv->display(buf);

}

void Inventory_Area::refresh()
{
	ska::Point<int> buf = m_relativePos;
	buf.x += (m_parent->getRect()).x;
	buf.y += (m_parent->getRect()).y;

	/*WGameCore& wScreen = WGameCore::getInstance();
	const ska::InputActionContainer& in = wScreen.getActions();
	const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];*/
	/*MouseCursor& mouseCur = wScreen.getMouseCursor();
	

	if (in[ska::InputAction::LClic] && ska::RectangleUtils::isPositionInBox(mousePos, buf) && !ska::RectangleUtils::isPositionInBox(mousePos, wScreen.getGUI().getClickMenu()->getRect()))
	{
		ska::Rectangle invAreaAbsolutePos = this->getAbsolutePos();
		//wScreen.getGUI().getClickMenu()->hide(true);
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

	}*/
}

Inventory_Area::~Inventory_Area()
{
}
