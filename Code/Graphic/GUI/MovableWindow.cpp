#include "MovableWindow.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\ChargementImages.h"
#include "../../Utils/IDs.h"
#include "../../ska/Utils/RectangleUtils.h"

using namespace std;

MovableWindow::MovableWindow(string fichierMenu, ska::Rectangle posFond, int taillePolice) :DialogMenu("", "", fichierMenu, posFond, taillePolice, false)
{
    m_mouseLastPos.x = m_mouseLastPos.y = 0;
}


void MovableWindow::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();
    ska::MouseInput *in = wScreen.getInputListener().getMouseInput();
	ska::Rectangle mousePos = in->getMousePos(), movePos = m_posFond;
    movePos.h = TAILLEBLOCFENETRE/2;
	if (in->mouseClick(SDL_BUTTON_LEFT) && ska::RectangleUtils::isPositionInBox(&mousePos, &movePos))
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
