#include <iostream>
#include <string>
#include "Button_Quit.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\ChargementImages.h"


using namespace std;

Button_Quit::Button_Quit(DialogMenu *parent, string imgName, string secondImgName, ska::Rectangle relativePos) : DynamicWindowArea(parent)
{
    m_type = BUTTON_CLOSE;
    m_active = parent->isVisible();
    m_relativePos.x = relativePos.x;
    m_relativePos.y = relativePos.y;
    m_sprite.load(imgName, T_RED, T_GREEN, T_BLUE, 128);
    m_spriteActive.load(secondImgName, T_RED, T_GREEN, T_BLUE, 128);
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
    MouseInput * in = wScreen.getInputListener().getMouseInput();

	ska::Rectangle buf = m_relativePos, mousePos = in->getMousePos();
    buf.x += (m_parent->getPos())->x;
    buf.y += (m_parent->getPos())->y;

    if(IsPositionInBox(&mousePos, &buf))
		m_spriteActive.render(buf.x, buf.y);
    else
        m_sprite.render(buf.x, buf.y);
    

}

void Button_Quit::refresh()
{
	if(!m_parent->isVisible())
		return;
	else
		m_active = true;

	WGameCore& wScreen = WGameCore::getInstance();
	MouseInput * in = wScreen.getInputListener().getMouseInput();

	ska::Rectangle buf = m_relativePos, mousePos = in->getMousePos();
	buf.x += (m_parent->getPos())->x;
	buf.y += (m_parent->getPos())->y;

	if(IsPositionInBox(&mousePos, &buf))
	{
		if(in->mouseClick(SDL_BUTTON_LEFT))
		{
			m_parent->hide(true);
			m_active = false;
			in->setMouseLastState(SDL_BUTTON_LEFT, 0);
			in->setMouseState(SDL_BUTTON_LEFT, 0);
		}
	}
}

Button_Quit::~Button_Quit()
{
}
