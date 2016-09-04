// #include "MovableWindow.h"
// #include "../../Utils\ChargementImages.h"
// #include "../../Utils/IDs.h"
// #include "../../ska/Utils/RectangleUtils.h"
// #include "../../ska/Inputs/InputContextManager.h"
// 
// MovableWindow::MovableWindow(const ska::InputContextManager& icm, const std::string& fichierMenu, ska::Rectangle posFond, int taillePolice) :
// DialogMenu("", "", fichierMenu, posFond, taillePolice, false),
// m_playerICM(icm) {
//     m_mouseLastPos.x = m_mouseLastPos.y = 0;
// }
// 
// 
// void MovableWindow::refresh() {
// 	const ska::InputToggleContainer& in = m_playerICM.getToggles();
// 	const ska::InputRange& mouseClickPos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
// 	auto movePos = getBox();
//     movePos.h = TAILLEBLOCFENETRE/2;
// 	
// 	if (in[ska::InputToggle::MoveWindow] && ska::RectangleUtils::isPositionInBox(mouseClickPos, movePos)) {
// 		m_moving = true;
// 	} else if (!in[ska::InputToggle::MoveWindow] && isMoving()) {
// 		m_moving = false;
// 	}
// 
//     if(isMoving()) {
// 		movePos.x += (int)(mouseClickPos.x - m_mouseLastPos.x);
// 		movePos.y += (int)(mouseClickPos.y - m_mouseLastPos.y);
//     }
// 	
// 	m_scrollingRect = movePos;
// 
// 	m_mouseLastPos = mouseClickPos;
//     DialogMenu::refresh();
// }
// 
// MovableWindow::~MovableWindow()
// {
// }
