#include <iostream>
#include <string>
#include "Button_Quit.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\ChargementImages.h"
#include "../../Utils/IDs.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "DialogMenu.h"

using namespace std;

Button_Quit::Button_Quit(DialogMenu *parent, string imgName, string secondImgName, ska::Rectangle relativePos) : DynamicWindowArea(parent)
{
    m_type = BUTTON_CLOSE;
    m_active = parent->isVisible();
    m_relativePos.x = relativePos.x;
    m_relativePos.y = relativePos.y;
    m_sprite.load(imgName, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128);
    m_spriteActive.load(secondImgName, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128);
    m_relativePos.w = m_sprite.getWidth();
    m_relativePos.h = m_sprite.getHeight();
}

void Button_Quit::display()
{
    if(!m_parent->isVisible())
        return;
    else
        m_active = true;

	WGameCore& wScreen = WGameCore::getInstance();
	const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];

	ska::Rectangle buf = m_relativePos;
	buf.x += (m_parent->getRect()).x;
	buf.y += (m_parent->getRect()).y;

	if (ska::RectangleUtils::isPositionInBox(mousePos, buf)) {
		m_spriteActive.render(buf.x, buf.y);
	} else {
		m_sprite.render(buf.x, buf.y);
	}
    

}

void Button_Quit::refresh()
{
	if(!m_parent->isVisible())
		return;
	else
		m_active = true;

	WGameCore& wScreen = WGameCore::getInstance();
	const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];
	const ska::InputActionContainer& in = wScreen.getActions();

	ska::Rectangle buf = m_relativePos;
	buf.x += (m_parent->getRect()).x;
	buf.y += (m_parent->getRect()).y;

	if (ska::RectangleUtils::isPositionInBox(mousePos, buf)) {
		if(in[ska::InputAction::LClic]) {
			m_parent->hide(true);
			m_active = false;
		}
	}
}

Button_Quit::~Button_Quit()
{
}
