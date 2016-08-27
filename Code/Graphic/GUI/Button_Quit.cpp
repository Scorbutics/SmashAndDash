// #include <iostream>
// #include <string>
// #include "Button_Quit.h"
// #include "../../ska/Inputs/InputContextManager.h"
// #include "../../Utils\ChargementImages.h"
// #include "../../Utils/IDs.h"
// #include "../../ska/Utils/RectangleUtils.h"
// #include "DialogMenu.h"
// 
// //TODO : � supprimer. button quit n'est qu'un button avec comme callback la fermeture de la fen�tre parente...
// Button_Quit::Button_Quit(DialogMenu& parent, const ska::InputContextManager& icm, const std::string& imgName, const std::string& secondImgName, ska::Rectangle relativePos) : DynamicWindowArea(parent),
// m_playerICM(icm) {
//     m_type = BUTTON_CLOSE;
// 	m_active = true;
//     m_relativePos.x = relativePos.x;
//     m_relativePos.y = relativePos.y;
//     m_sprite.load(imgName, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128);
//     m_spriteActive.load(secondImgName, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128);
// }
// 
// void Button_Quit::display() const
// {
//     if(!m_parent.isVisible())
//         return;
// 	
// 	const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
// 
// 	ska::Rectangle buf;
// 	buf.x = m_relativePos.x + (m_parent.getRect()).x;
// 	buf.y = m_relativePos.y + (m_parent.getRect()).y;
// 	buf.w = m_sprite.getWidth();
// 	buf.h = m_sprite.getHeight();
// 
// 	if (ska::RectangleUtils::isPositionInBox(mousePos, buf)) {
// 		m_spriteActive.render(buf.x, buf.y);
// 	} else {
// 		m_sprite.render(buf.x, buf.y);
// 	}
//     
// 
// }
// 
// void Button_Quit::refresh()
// {
// 	if(!m_parent.isVisible())
// 		return;
// 	else
// 		m_active = true;
// 
// 	const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
// 	const ska::InputActionContainer& in = m_playerICM.getActions();
// 
// 	ska::Rectangle buf;
// 	buf.x = m_relativePos.x + (m_parent.getRect()).x;
// 	buf.y = m_relativePos.y + (m_parent.getRect()).y;
// 	buf.w = m_sprite.getWidth();
// 	buf.h = m_sprite.getHeight();
// 
// 	if (ska::RectangleUtils::isPositionInBox(mousePos, buf)) {
// 		if(in[ska::InputAction::LClic]) {
// 			m_parent.hide(true);
// 			m_active = false;
// 		}
// 	}
// }
// 
// Button_Quit::~Button_Quit()
// {
// }
