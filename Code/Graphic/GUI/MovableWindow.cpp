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
	const ska::InputToggleContainer& in = wScreen.getToggles();
	const ska::InputRange& mouseClickPos = wScreen.getRanges()[ska::InputRangeType::MousePos];
	ska::Rectangle movePos = m_rect;
    movePos.h = TAILLEBLOCFENETRE/2;
	
	if (in[ska::InputToggle::MoveWindow] && ska::RectangleUtils::isPositionInBox(mouseClickPos, movePos)) {
		m_moving = true;
	} else if (!in[ska::InputToggle::MoveWindow] && isMoving()) {
		m_moving = false;
	}

    if(isMoving()) {
		m_rect.x += mouseClickPos.x - m_mouseLastPos.x;
		m_rect.y += mouseClickPos.y - m_mouseLastPos.y;
    }

	m_mouseLastPos = mouseClickPos;
    DialogMenu::refresh();
}

MovableWindow::~MovableWindow()
{
}
