#include "MovableWindow.h"
#include "Gameplay\WGameCore.h"
#include "Utils\ChargementImages.h"

using namespace std;

MovableWindow::MovableWindow(string fichierMenu, SDL_Rect posFond, int taillePolice):DialogMenu("", "", fichierMenu, posFond, taillePolice, false)
{
    m_mouseLastPos.x = m_mouseLastPos.y = 0;
}


void MovableWindow::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();
    MouseInput *in = wScreen.getInputListener().getMouseInput();
    SDL_Rect mousePos = in->getMousePos(), movePos = m_posFond;
    movePos.h = TAILLEBLOCFENETRE/2;
    if(in->mouseClick(SDL_BUTTON_LEFT) && IsPositionInBox(&mousePos, &movePos))
	{
        this->setMoving(true);
		in->setMouseLastState(SDL_BUTTON_LEFT, 1);
	}
    else if(!in->getMouseState(SDL_BUTTON_LEFT))
        this->setMoving(false);

    if(this->isMoving())
    {
        m_posFond.x += in->getMousePos().x - m_mouseLastPos.x;
        m_posFond.y += in->getMousePos().y - m_mouseLastPos.y;
    }

    m_mouseLastPos = in->getMousePos();
    DialogMenu::refresh();
}

MovableWindow::~MovableWindow()
{
}
