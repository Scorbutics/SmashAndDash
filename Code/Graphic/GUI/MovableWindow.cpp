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
	const ska::InputActionContainer& in = wScreen.getActions();
	const ska::InputRange& mouseClickPos = wScreen.getRanges()[ska::InputRangeType::MousePos];
	ska::Rectangle movePos = m_rect;
    movePos.h = TAILLEBLOCFENETRE/2;
	
	if (in[ska::InputAction::LClic] && ska::RectangleUtils::isPositionInBox(mouseClickPos, movePos)) {
        this->setMoving(true);
	} else if (!in[ska::InputAction::LClic]) {
		this->setMoving(false);
	}

    if(this->isMoving()) {
		m_rect.x += mouseClickPos.x - m_mouseLastPos.x;
		m_rect.y += mouseClickPos.y - m_mouseLastPos.y;
    }

	m_mouseLastPos = mouseClickPos;
    DialogMenu::refresh();
}

MovableWindow::~MovableWindow()
{
}
