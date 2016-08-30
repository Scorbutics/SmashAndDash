#include <string>
#include <fstream>

#include "GUI.h"
#include "../../Utils\SkaConstants.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/RectangleUtils.h"
#include "../../Inputs/InputContextManager.h"
#include "../../Inputs/InputAction.h"
#include "../../Inputs/InputRange.h"
#include "../../Inputs/InputToggle.h"
#include "../../Graphic/GUI/Window.h"
#include "./Components/Widget.h"
#include "./Components/HoverEvent.h"
#include "./Components/ClickEvent.h"
#include "WindowIG.h"
#include "./Components/Button.h"

#define SCROLL_BUTTON_SPEED 3

ska::GUI::GUI(const ska::Window& w, const ska::InputContextManager& playerICM) :
m_playerICM(playerICM),
m_window(w),
m_wAction(ska::Rectangle{ 0, 0, 13 * TAILLEBLOCFENETRE, 2 * TAILLEBLOCFENETRE }, false) {

    //m_refreshCount = REFRESH_PNJWINDOW_COUNT;
    m_lastMouseState = 0;
    m_side = 0;
    //m_pnj = NULL;
    m_isMovingWindow = m_hide = false;

// 	ska::Rectangle menuPos;
// 	menuPos.x = 0;
//     menuPos.y = 0;
//     menuPos.w = 8*TAILLEBLOCFENETRE;
//     menuPos.h = 5*TAILLEBLOCFENETRE;
//     m_pokeInfoWindow = std::unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));
// 
//     menuPos.h = 4*TAILLEBLOCFENETRE;
//     menuPos.y = 5*TAILLEBLOCFENETRE;
//     menuPos.w = 5*TAILLEBLOCFENETRE;
// 	m_facesetPkmn = std::unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));
// 
//     menuPos.x = w.getWidth() - 2*TAILLEBLOCFENETRE;
// 	m_attackOpponent = std::unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));
// 
// 
//     menuPos.x = 0;
//     menuPos.y = 0*TAILLEBLOCFENETRE;
//     menuPos.w = 6*TAILLEBLOCFENETRE;
//     menuPos.h = 3*TAILLEBLOCFENETRE;
// 	m_pnjInfoWindow = std::unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));
// 
//     menuPos.y = 5*TAILLEBLOCFENETRE;
//     menuPos.x += 3*TAILLEBLOCFENETRE;
//     menuPos.h = 4*TAILLEBLOCFENETRE;
//     menuPos.w = 5*TAILLEBLOCFENETRE;
// 	m_facesetOpponent = std::unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));
// 
//     menuPos.x = menuPos.y = 0;
//     menuPos.w = 4*TAILLEBLOCFENETRE;
//     menuPos.h = 2*TAILLEBLOCFENETRE;
// 	m_clickMenu = std::unique_ptr<DialogMenu>(new DialogMenu("", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22, false));
// 
// 
//     menuPos.w = 10*TAILLEBLOCFENETRE;
//     menuPos.h = 5*TAILLEBLOCFENETRE;
// 	m_wSettings = std::unique_ptr<WindowSettings>(new WindowSettings(playerICM, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22));
// 	m_wTeam = std::unique_ptr<WindowTeam>(new WindowTeam(playerICM, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22));
// 	m_wBag = std::unique_ptr<WindowBag>(new WindowBag(playerICM, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22));
// 
// 	menuPos.x = w.getWidth() - 7*TAILLEBLOCFENETRE;
// 	menuPos.y = 0;
// 	menuPos.w = 7*TAILLEBLOCFENETRE;
// 	menuPos.h = 2*TAILLEBLOCFENETRE;
// 	m_toolBar = std::unique_ptr<ToolBar>(new ToolBar(playerICM, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square_highlight.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos));
// 	
// 	menuPos.x = 5*TAILLEBLOCFENETRE;
// 	menuPos.y = 2*TAILLEBLOCFENETRE;
// 	menuPos.w = 10*TAILLEBLOCFENETRE;
// 	menuPos.h = 10*TAILLEBLOCFENETRE;
// 	m_wShop = std::unique_ptr<WindowShop>(new WindowShop(playerICM, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"inventory_square_highlight.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos));
//     m_toolBar->hide(false);

    initButtons(w);

	setPriority(INT_MAX);

}

void ska::GUI::initButtons(const ska::Window& w) {
	ska::Rectangle buf;
    buf.w = (TAILLEBLOCFENETRE)*2;
    buf.h = (TAILLEBLOCFENETRE)*2;
    buf.x = 0;
    buf.y = 0;

    m_wAction.clear();
	m_wAction.move(ska::Point<int>(w.getWidth() - 13 * TAILLEBLOCFENETRE, w.getHeight() - m_wAction.getBox().h / 2));
    //m_buttonList.push_back(DialogMenuPtr (new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"pokeball.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
	auto& firstButton = std::unique_ptr<ska::WindowIG>(new ska::WindowIG(m_wAction, buf, true));
	firstButton->addWidget(std::move(std::unique_ptr<ska::Widget>(new Button(*this, *firstButton, Point<int>(0, 0), "", [](const ska::ClickEvent& e) {
		std::clog << "BOUH !" << std::endl;
	}))));
	m_wAction.addWidget(std::move(firstButton));
	//m_buttonList[0]->name("Equipe");
	/*m_buttonList[0]->setClickHandler([&] {
		m_wTeam->setPos(ska::Point<int>());
		m_wTeam->reset("show");
		m_wTeam->hide(false);
	});*/

    buf.x += 5*TAILLEBLOCFENETRE/2;
	//m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"pokedex.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
	m_wAction.addWidget(std::unique_ptr<ska::Widget>(new ska::WindowIG(m_wAction, buf, true)));
	//m_buttonList[1]->name("Pokédex");
	//m_buttonList[1]->setActionClic("pokedex");

    buf.x += 5*TAILLEBLOCFENETRE/2;
	//m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"bag.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
	m_wAction.addWidget(std::unique_ptr<ska::Widget>(new ska::WindowIG(m_wAction, buf, true)));
	//m_buttonList[2]->name("PokéSac");
	/*m_buttonList[2]->setClickHandler([&] {
		m_wBag->setPos(ska::Point<int>(m_wTeam->getRect().w, 0));
		m_wBag->reset();
		m_wBag->hide(false);
	});*/

    buf.x += 5*TAILLEBLOCFENETRE/2;
	//m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"card.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
	m_wAction.addWidget(std::unique_ptr<ska::Widget>(new ska::WindowIG(m_wAction, buf, true)));
    //m_buttonList[3]->setActionClic("trainer_card");
	//m_buttonList[3]->name("Carte dresseur");

    buf.x += 5*TAILLEBLOCFENETRE/2;
	//m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"tape.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
	m_wAction.addWidget(std::unique_ptr<ska::Widget>(new ska::WindowIG(m_wAction, buf, true)));
	//m_buttonList[4]->name("Paramètres");
	/*m_buttonList[4]->setClickHandler([&] {
		m_wSettings->setPos(ska::Point<int>(m_wTeam->getRect().w + m_wBag->getRect().w, 0));
		m_wSettings->reset();
		m_wSettings->hide(false);
	});*/

    /*m_buttonScroll.resize(m_buttonList.size());
	for (unsigned int i = 0; i < m_buttonList.size(); i++) {
		m_buttonScroll[i] = 0;
		m_buttonList[i]->hide(false);
	}*/


}

void ska::GUI::display() const {

	//dialogDisplay();

	if (m_hide) {
		return;
	}

	/*for (unsigned int i = 0; i < m_buttonList.size(); i++) {
		if (m_buttonList[i] != NULL) {
			m_buttonList[i]->display();
		}
	}*/

	m_wAction.display();

// 	m_mouseCursor.displayHint();
// 	m_mouseCursor.displaySelectedPokemon();
// 	m_mouseCursor.displaySelectedObject();
		
}

void ska::GUI::refresh() {
	
	const ska::InputActionContainer& in = m_playerICM.getActions();

	const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
	const ska::InputRange& lastMousePos = m_playerICM.getRanges()[ska::InputRangeType::LastMousePos];
	ska::Rectangle bufButtonPos;

    bufButtonPos.w = (TAILLEBLOCFENETRE)*2;
    bufButtonPos.h = (TAILLEBLOCFENETRE)*2;
	bufButtonPos.x = m_window.getWidth() - 13 * TAILLEBLOCFENETRE;
    bufButtonPos.y = 0;

	if (m_hide) {
		return;
	}
	
	int hideHint = 0;

	const auto& pointedElement = m_wAction.getElementAtPos(mousePos);
	if (pointedElement != nullptr) {
		HoverEvent he(MouseEventType::MOUSE_HOVER);
		HoverObservable::notifyObservers(he);
		//pointedElement->

		if (in[ska::InputAction::LClic]) {
			ClickEvent ce(MouseEventType::MOUSE_CLICK, ska::Point<int>(mousePos));
			ClickObservable::notifyObservers(ce);
		} 
	}

	if (!in[ska::InputAction::LClic]) {
		ClickEvent ce(MouseEventType::MOUSE_RELEASE, ska::Point<int>(mousePos));
		ClickObservable::notifyObservers(ce);
	}

// 	for(unsigned int i = 0; i < m_buttonList.size(); i++)
//     {
//         if(m_buttonList[i] != NULL) {
//             m_buttonList[i]->refresh();
// 			ska::Rectangle buttonPos = m_buttonList[i]->getBox();
// 			if (ska::RectangleUtils::isPositionInBox(mousePos, buttonPos)) {
// 
// 				if (m_buttonScroll[i] < buttonPos.h / 2) {
// 					m_buttonScroll[i] += SCROLL_BUTTON_SPEED;
// 				}
// 
//                 /*if(in[ska::InputAction::LClic] && !m_wShop->isVisible()) {
// 					m_buttonList[i]->click(mousePos);
//                 }*/
// 
// 				//si on change de bouton de la gui, on actualise le curseur
// 				if (!ska::RectangleUtils::isPositionInBox(lastMousePos, buttonPos)) {
// 					//m_mouseCursor.modifyHint(m_buttonList[i]->getName());
// 					m_mouseCursor.hideHint(false);
// 					
// 				}
// 				                
//             } else {
// 				hideHint++;
// 				m_buttonScroll[i] = 0;
// 			}
// 
// 			// GUI disposée en bas à droite de l'écran
//             if(m_side == 0)  {
// 				m_buttonList[i]->move(ska::Point<int>(bufButtonPos.x, (m_window.getHeight() - m_buttonList[i]->getBox().h / 2) - m_buttonScroll[i]));
// 				//m_buttonList[i]->setPosImg(ska::RectangleUtils::posToCenterPicture(m_buttonList[i]->getPosImg(), bufButtonPos).x, ska::RectangleUtils::posToCenterPicture(m_buttonList[i]->getPosImg(), m_buttonList[i]->getRect()).y - TAILLEBLOCFENETRE / 4);
//                 bufButtonPos.x += 5*TAILLEBLOCFENETRE/2;
//             }
// 
// 
//         }
//     }


	if (in[ska::InputAction::RClic]) {
		setClickMenu();
	}

// 	if (hideHint == m_buttonScroll.size()) {
// 		m_mouseCursor.hideHint(true);
// 	}
// 
// 	m_mouseCursor.update();

}

// int ska::GUI::addDialog(IDialogMenuPtr& d) {
// 	d->hide(false);
// 	m_extraWindows.emplace_back(std::move(d));
// 	return (int)(m_extraWindows.size() - 1);
// }

void ska::GUI::setClickMenu() {
	//ska::Rectangle invAreaAbsolutePos, buttonPos;

	const ska::InputActionContainer& in = m_playerICM.getActions();

	const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
	const ska::InputRange& lastMousePos = m_playerICM.getRanges()[ska::InputRangeType::LastMousePos];

    std::vector<int> vBool;
    vBool.push_back(0);
    vBool.push_back(1);

    /*Inventory_Area* invArea = m_wBag->getInventoryArea(0);
    if(invArea != NULL && m_wBag->isVisible()) {
        invAreaAbsolutePos = invArea->getRect();
		if (ska::RectangleUtils::isPositionInBox(mousePos, invAreaAbsolutePos) && !ska::RectangleUtils::isPositionInBox(mousePos, m_clickMenu->getRect())) {
			ska::Point<int> posClickMenu = mousePos;
            posClickMenu.x -= 5;
            posClickMenu.y -= 5;
            m_clickMenu->setPos(posClickMenu);
            m_curObjectPos = m_clickMenu->getRect();
            m_curObjectPos.x -= invAreaAbsolutePos.x;
            m_curObjectPos.y -= invAreaAbsolutePos.y;
            if(invArea->getObjectAtPos(m_curObjectPos) != NULL) {
				std::vector<std::string> vUse, vGive, vTrash;
                vUse.push_back("Utiliser");
                vUse.push_back("Ok !");
                vGive.push_back("Donner");
                vGive.push_back("Ok !");
                vTrash.push_back("Jeter");
                vTrash.push_back("Ok !");

                m_clickMenu->deleteAll();
                buttonPos.x = 32;
                buttonPos.y = 16;
                m_clickMenu->addButton(buttonPos, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenu.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenupressed.png", m_wBag->getBoolUseObject(), vBool, vUse, 18, "inventory_menu_use");
                buttonPos.y += 15;
                m_clickMenu->addButton(buttonPos, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenu.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenupressed.png", m_wBag->getBoolGiveObject(), vBool, vGive, 18, "inventory_menu_give");
                buttonPos.y += 15;
                m_clickMenu->addButton(buttonPos, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenu.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonclickmenupressed.png", m_wBag->getBoolTrashObject(), vBool, vTrash, 18, "inventory_menu_trash");
                m_clickMenu->hide(false);
                invArea = m_wBag->getInventoryArea(0);
            } else {
				m_clickMenu->hide(true);
			}



        } else if (!ska::RectangleUtils::isPositionInBox(mousePos, m_clickMenu->getRect())) {
            m_clickMenu->hide(true);
            *m_wBag->getBoolUseObject() = 0;
            *m_wBag->getBoolTrashObject() = 0;
            *m_wBag->getBoolGiveObject() = 0;
        }

    }
	*/
}


bool ska::GUI::existDialog(const unsigned int index) const {
	return index < m_extraWindows.size() && m_extraWindows[index] != nullptr;
}

//Reset des informations de notre pokémon (à chaque refresh)
//TODO
/*void GUI::resetAttackPokemonWindow(Character* pokemon)
{
    m_attackPokemon->deleteAll();
	ska::Rectangle buf, bufNULL;
    buf.x = TAILLEBLOCFENETRE/4;
    buf.y = 0;
    bufNULL.x = 0;
    bufNULL.y = 0;
    vector<Skill_ptr>* v = pokemon->getSkills();

	
    if(v != NULL && v->size() > 0)
    {
        bufNULL.w = (*v)[0]->getIcon()->getWidth();
        bufNULL.h = (*v)[0]->getIcon()->getHeight();
    
		for(unsigned int i = 0; i < (*v).size(); i++)
		{
			buf.x += (*v)[i]->getIcon()->getWidth();
			m_attackPokemon->addImageArea((*v)[i]->getIcon(), false, buf, &bufNULL);
		}
	}

}

//Reset des informations pokémon adverse (à chaque refresh)
void GUI::resetAttackOpponentWindow(Character* op)
{
    m_attackOpponent->deleteAll();
	ska::Rectangle buf, bufNULL;
    buf.x = TAILLEBLOCFENETRE/4;
    buf.y = 0;
    bufNULL.x = 0;
    bufNULL.y = 0;
    vector<Skill_ptr>* v = op->getSkills();
    if(v != NULL && v->size() > 0)
    {
        bufNULL.w = (*v)[0]->getIcon()->getWidth();
        bufNULL.h = (*v)[0]->getIcon()->getHeight();
    
		for(unsigned int i = 0; i < (*v).size(); i++)
		{
			buf.y += (*v)[i]->getIcon()->getHeight();
			m_attackOpponent->addImageArea((*v)[i]->getIcon(), false, buf, &bufNULL);

		}
	}
}*/

// size_t GUI::getButtonListSize() {
//     return m_buttonList.size();
// }

/*
void GUI::resetInfoPokemonWindow(Character* pokemon)
{
    int id;
    stringstream ss;
	WGameCore& wScreen = WGameCore::getInstance();
	ska::Rectangle buf, posHero, oRel = { 0 };

    m_pokeInfoWindow->deleteAll();
    m_pnj = pokemon;

    if(m_pnj != NULL) {
        posHero.x = (pokemon->getCenterPos().x + abs(oRel.x))/TAILLEBLOCFENETRE;
        posHero.y = (pokemon->getCenterPos().y + abs(oRel.y))/TAILLEBLOCFENETRE;

        buf.x = 6*TAILLEBLOCFENETRE+TAILLEBLOCFENETRE/2;
        buf.y = TAILLEBLOCFENETRE*3/4;
        m_pokeInfoWindow->addButtonClose("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button_active.png", buf);
        buf.x = TAILLEBLOCFENETRE/2;
        buf.y = TAILLEBLOCFENETRE/2;
        m_pokeInfoWindow->addTextArea("Type 1 : " + pokemon->getDescriptor()->getType(1), 20, buf);
        buf.x += 3*TAILLEBLOCFENETRE-10;
        m_pokeInfoWindow->addTextArea("Type 2 : " + pokemon->getDescriptor()->getType(1), 20, buf);
        buf.x -= 3*TAILLEBLOCFENETRE-10;
        buf.y = TAILLEBLOCFENETRE/2 + 20;
        m_pokeInfoWindow->addTextArea("Nom : " + pokemon->getDescriptor()->getName(), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 60;
        m_pokeInfoWindow->addTextArea("Level : " + ska::StringUtils::intToStr(pokemon->getStatistics()->getLevel()), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 80;
		m_pokeInfoWindow->addTextArea("HP : " + ska::StringUtils::intToStr(pokemon->getHp()), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 100;
		m_pokeInfoWindow->addTextArea("Position : " + ska::StringUtils::intToStr(posHero.x) + ":" + ska::StringUtils::intToStr(posHero.y), 20, buf);

        m_facesetPkmn->deleteAll();
        id = pokemon->getID();
		if (id >= 0) {
			ss << "."FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" << id << ".png";
		} else {
			ss << "."FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" << -id << ".png";
		}

        buf.x = TAILLEBLOCFENETRE/2;
        buf.y = TAILLEBLOCFENETRE/2;
        m_facesetPkmn->addImageArea(ss.str(), true, buf, NULL);
    }
}
*/
/*
void GUI::resetInfoPNJWindow(Character* pnj)
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::Rectangle buf, posHero, oRel = { 0 };
    stringstream ss;
    int id;
    m_pnjInfoWindow->deleteAll();
    m_pnj = pnj;

    if(m_pnj != NULL) {
        posHero.x = (pnj->getCenterPos().x + abs(oRel.x))/TAILLEBLOCFENETRE;
        posHero.y = (pnj->getCenterPos().y + abs(oRel.y))/TAILLEBLOCFENETRE;

        buf.x = 4*TAILLEBLOCFENETRE+TAILLEBLOCFENETRE/2;
        buf.y = TAILLEBLOCFENETRE/2;
        m_pnjInfoWindow->addButtonClose("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"close_button_active.png", buf);
		ska::Rectangle rectSrcBuf;
        rectSrcBuf.x = 0;
        rectSrcBuf.y = pnj->getSprite()->getHeight()*2 /8;
        rectSrcBuf.w = pnj->getSprite()->getWidth()/3;
        rectSrcBuf.h = pnj->getSprite()->getHeight()/8;
        m_pnjInfoWindow->addImageArea(pnj->getSprite(), false, buf, &rectSrcBuf);
        buf.x = TAILLEBLOCFENETRE/2;
        
        m_pokeInfoWindow->addTextArea("Type 1 : " + pnj->getType(1), 20, buf);
        buf.x += 3*TAILLEBLOCFENETRE;
        m_pokeInfoWindow->addTextArea("Type 2 : " + pnj->getType(1), 20, buf);
        buf.x -= 3*TAILLEBLOCFENETRE;
        buf.y = TAILLEBLOCFENETRE/2 + 20;
        m_pnjInfoWindow->addTextArea(pnj->getDescriptor()->getName(), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 20;
		m_pnjInfoWindow->addTextArea("Level : " + ska::StringUtils::intToStr(pnj->getStatistics()->getLevel()), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 40;
		m_pnjInfoWindow->addTextArea("HP : " + ska::StringUtils::intToStr(pnj->getHp()) + "/" + ska::StringUtils::intToStr(pnj->getStatistics()->getHpMax()), 20, buf);
        buf.y = TAILLEBLOCFENETRE/2 + 60;
		m_pnjInfoWindow->addTextArea("Position : " + ska::StringUtils::intToStr(posHero.x) + ":" + ska::StringUtils::intToStr(posHero.y), 20, buf);

        m_facesetOpponent->deleteAll();
        id = pnj->getID();
		if (id >= 0) {
			ss << "."FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" << id << ".png";
		} else {
			ss << "."FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" << -id << ".png";
		}

        buf.x = TAILLEBLOCFENETRE/2;
        buf.y = TAILLEBLOCFENETRE/2;
        m_facesetOpponent->addImageArea(ss.str(), false, buf, NULL);
    }
}*/

/*void GUI::update(ska::Observable<const int>* obs, const ska::EventArg& e, const int& i) {
}*/

void ska::GUI::update() {
	m_hide = false;

	std::vector<std::vector<std::unique_ptr<ska::WindowIG>>::iterator> toDelete;
	unsigned int index = 0;
	for (auto& w = m_extraWindows.begin(); w != m_extraWindows.end(); w++) {
		if ((*w)->isVisible()) {
			//m_hide = true;
			(*w)->display();
		}
		else {
			toDelete.push_back(w);
		}
		index++;
	}

	for (auto& d : toDelete) {
		m_extraWindows.erase(d);
	}
}

//TODO
// void ska::GUI::dialogDisplay() const {
// 	
// 	if (m_pnjInfoWindow->isVisible()) {
// 		m_pnjInfoWindow->display();
// 	}
// 
// 	if (m_wSettings->isVisible()) {
// 		m_wSettings->display();
// 	}
// 
// 	if (m_wBag->isVisible()) {
// 		m_wBag->display();
// 	}
// 
// 	if (m_wTeam->isVisible()) {
// 		m_wTeam->display();
// 	}
// 
//     if(m_clickMenu->isVisible()) {
// 		const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
// 		if (ska::RectangleUtils::isPositionInBox(mousePos, m_clickMenu->getBox())) {
// 			m_clickMenu->display();
// 		} else {
//             m_clickMenu->move(ska::Point<int>());
//             m_clickMenu->hide(true);
//         }
// 
//     }
// 
// 	if (m_toolBar->isVisible()) {
// 		m_toolBar->display();
// 	}
// 
// 	if (m_wShop->isVisible()) {
// 		m_wShop->display();
// 	}
// 
// }

// 
// void ska::GUI::dialogRefresh() {
// 
// 	for (auto& w : m_extraWindows) {
// 		if (w->isVisible()) {
// 			//m_hide = true;
// 			w->refresh();
// 		}
// 	}
// 
// 	if (m_pnjInfoWindow->isVisible()) {
// 		m_pnjInfoWindow->refresh();
// 	}
// 
// 	if (m_wSettings->isVisible()) {
// 		m_wSettings->refresh();
// 	}
// 
// 	if (m_wBag->isVisible()) {
// 		m_wBag->refresh();
// 	}
// 
// 	if (m_wTeam->isVisible()) {
// 		m_wTeam->refresh();
// 	}
// 
// 	if(m_clickMenu->isVisible()) {
// 		const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
// 		
// 		if (ska::RectangleUtils::isPositionInBox(mousePos, m_clickMenu->getBox())) {
// 			m_clickMenu->refresh();
// 		} else {
// 			m_clickMenu->move(ska::Point<int>());
// 			m_clickMenu->hide(true);
// 		}
// 
// 	}
// 
// 
// 	if (m_toolBar->isVisible()) {
// 		m_toolBar->refresh();
// 	}
// 
// 	if (m_wShop->isVisible()) {
// 		m_wShop->refresh();
// 	}

//}

//GUI::isPositionOnButton renvoie l'indice du bouton où se situe "pos" dans m_buttonList
//Renvoie -1 si pos n'est dans aucun bouton
int ska::GUI::isPositionOnButton(const ska::Point<float>& pos) const {
// 	for (unsigned int i = 0; i < m_buttonList.size(); i++) {
// 		if (ska::RectangleUtils::isPositionInBox(pos, m_buttonList[i]->getBox())) {
// 			return i;
// 		}
// 	}
    return -1;
}

void ska::GUI::hide(bool x) {
    m_hide = x;
}

bool ska::GUI::isVisible() const {
    return !m_hide;
}

// DialogMenuPtr& GUI::getButton(unsigned int id) {
//     return m_buttonList[id];
// }

bool ska::GUI::isMovingAWindow() {
	return false;//((m_wSettings->isVisible() && m_wSettings->isMoving()) || (m_wBag->isVisible() && m_wBag->isMoving()) || (m_wTeam->isVisible() && m_wTeam->isMoving()) || ( m_toolBar->isVisible() && m_toolBar->isMoving()) );
}

bool ska::GUI::isMouseOnAWindow() {
	const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];

	return false;
// 	return ((m_wBag->isVisible() && ska::RectangleUtils::isPositionInBox(mousePos, m_wBag->getBox())) 
// 		|| (m_wSettings->isVisible() && ska::RectangleUtils::isPositionInBox(mousePos, m_wSettings->getBox())) 
// 		|| (m_wTeam->isVisible() && ska::RectangleUtils::isPositionInBox(mousePos, m_wTeam->getBox()))
// 		|| (m_toolBar->isVisible() && ska::RectangleUtils::isPositionInBox(mousePos, m_toolBar->getBox())));
}

// int ska::GUI::getRefreshPNJWindowCount() {
//     return m_refreshCount;
// }
// 
// void ska::GUI::setRefreshPNJWindowCount(int x) {
//     m_refreshCount = x;
// }
