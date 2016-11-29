#include <string>
#include <fstream>
#include <algorithm>

#include "GUI.h"
#include "../../Utils/SkaConstants.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/RectangleUtils.h"
#include "../../Inputs/InputContextManager.h"
#include "../../Inputs/InputAction.h"
#include "../../Inputs/InputRange.h"
#include "../../Inputs/InputToggle.h"
#include "../../Graphic/GUI/Window.h"
#include "./Components/Widget.h"
#include "./Components/HoverEvent.h"
#include "./Components/KeyEvent.h"
#include "./Components/FocusEvent.h"
#include "./Components/ClickEvent.h"
#include "GUIScrollButtonWindowIG.h"
#include "./Components/ButtonSprite.h"

#include "./Components/HoverEventListener.h"

#define SCROLL_BUTTON_SPEED 3

ska::GUI::GUI(const ska::Window& w, ska::InputContextManager& playerICM) :
m_playerICM(playerICM),
m_window(w),
m_hovered(nullptr),
m_clicked(nullptr),
m_wMaster(this, this, ska::Rectangle{ 0, 0, w.getWidth(), w.getHeight() }, false) {

	m_wAction = new DynamicWindowIG<>(m_wMaster, ska::Rectangle{ 0, 0, 13 * TAILLEBLOCFENETRE, 2 * TAILLEBLOCFENETRE }, false);

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

	setPriority(std::numeric_limits<int>().max());
	m_wAction->setPriority(0);
	addWindow(std::unique_ptr<DynamicWindowIG<>>(m_wAction), "actions");
}

void ska::GUI::initButtons(const ska::Window& w) {
	ska::Rectangle buf;
    buf.w = (TAILLEBLOCFENETRE)*2;
    buf.h = (TAILLEBLOCFENETRE)*2;
    buf.x = 0;
    buf.y = 0;

	m_wAction->clear();
	m_wAction->move(ska::Point<int>(w.getWidth() - 13 * TAILLEBLOCFENETRE, w.getHeight() - m_wAction->getBox().h / 2));

	auto scrollButtonLambda = [this](ska::Widget* tthis, ska::HoverEvent& e) {
		auto target = ((GUIScrollButtonWindowIG*)tthis);
		if (e.getState() == ska::MouseEventType::MOUSE_OUT) {
			if (target->scrollRewind()) {
				m_dynamicWindows.emplace(target);
			}
		} else if (e.getState() == ska::MouseEventType::MOUSE_ENTER) {
			if (target->scrollTo(ska::Point<int>(target->getRelativePosition().x, -target->getBox().w / 3), 5)) {
				m_dynamicWindows.emplace(target);
			}
		}

	};

	auto firstButton = std::unique_ptr<GUIScrollButtonWindowIG>(new GUIScrollButtonWindowIG(*m_wAction, buf, true));	
	std::unique_ptr<ButtonSprite> bs = std::unique_ptr<ButtonSprite>(new ButtonSprite(*firstButton, Point<int>(1, 1), "", 102, [&](ska::Widget* tthis, const ska::ClickEvent& e) {
		if (e.getState() == ska::MouseEventType::MOUSE_CLICK) {
			auto w = m_windowAnnuary["team"];
			w->show(!w->isVisible());
		}
	}));
	bs->setName("POKEBALL BUTTON");
	firstButton->addWidget(bs);
	firstButton->setName("POKEBALL MENU");

	firstButton->addHandler<HoverEventListener>(scrollButtonLambda);
	m_wAction->addWidget(firstButton);
	m_wAction->setName("ACTIONS");

    buf.x += 5*TAILLEBLOCFENETRE/2;
	//m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"pokedex.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
	auto secondButton = std::unique_ptr<GUIScrollButtonWindowIG>(new GUIScrollButtonWindowIG(*m_wAction, buf, true));
	secondButton->addHandler<HoverEventListener>(scrollButtonLambda);
	m_wAction->addWidget(secondButton);
	//m_buttonList[1]->name("Pokédex");
	//m_buttonList[1]->setActionClic("pokedex");

    buf.x += 5*TAILLEBLOCFENETRE/2;
	//m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"bag.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
	auto thirdButton = std::unique_ptr<GUIScrollButtonWindowIG>(new GUIScrollButtonWindowIG(*m_wAction, buf, true));
	thirdButton->addHandler<HoverEventListener>(scrollButtonLambda);
	m_wAction->addWidget(thirdButton);
	//m_buttonList[2]->name("PokéSac");
	/*m_buttonList[2]->setClickHandler([&] {
		m_wBag->setPos(ska::Point<int>(m_wTeam->getRect().w, 0));
		m_wBag->reset();
		m_wBag->hide(false);
	});*/

    buf.x += 5*TAILLEBLOCFENETRE/2;
	//m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"card.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
	auto fourthButton = std::unique_ptr<GUIScrollButtonWindowIG>(new GUIScrollButtonWindowIG(*m_wAction, buf, true));
	fourthButton->addHandler<HoverEventListener>(scrollButtonLambda);
	m_wAction->addWidget(fourthButton);
    //m_buttonList[3]->setActionClic("trainer_card");
	//m_buttonList[3]->name("Carte dresseur");

    buf.x += 5*TAILLEBLOCFENETRE/2;
	//m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"tape.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
	auto fifthButton = std::unique_ptr<GUIScrollButtonWindowIG>(new GUIScrollButtonWindowIG(*m_wAction, buf, true));
	fifthButton->addHandler<HoverEventListener>(scrollButtonLambda);
	std::unique_ptr<ButtonSprite> bsFifth = std::unique_ptr<ButtonSprite>(new ButtonSprite(*fifthButton, Point<int>(1, 1), "", 104, [&](ska::Widget* tthis, const ska::ClickEvent& e) {
		if (e.getState() == ska::MouseEventType::MOUSE_CLICK) {
			auto w = m_windowAnnuary["settings"];
			w->show(!w->isVisible());
		}
	}));
	fifthButton->addWidget(std::move(bsFifth));
	m_wAction->addWidget(fifthButton);
	//m_buttonList[4]->name("Paramètres");
	/*m_buttonList[4]->setClickHandler([&] {
		m_wSettings->setPos(ska::Point<int>(m_wTeam->getRect().w + m_wBag->getRect().w, 0));
		m_wSettings->reset();
		m_wSettings->hide(false);
	});*/


}

void ska::GUI::display() const {

	//dialogDisplay();

	if (m_hide) {
		return;
	}

	//m_wAction.display();
	m_wMaster.display();
	/*if (!m_extraWindows.empty()) {
		std::vector<DynamicWindowIG<>*> drawables(m_extraWindows.size());
		for(auto& w : m_extraWindows) {
			drawables.push_back(w.get());
		}

		std::sort(drawables.begin(), drawables.end(), ska::Drawable::staticOperatorInf);

		for (const auto& w : drawables) {
			w->display();
		}
	}*/
// 	m_mouseCursor.displayHint();
// 	m_mouseCursor.displaySelectedPokemon();
// 	m_mouseCursor.displaySelectedObject();

}

void ska::GUI::refreshKeyboard() {
	const auto& textTyped = m_playerICM.getTextInput();

	const auto& isDeleting = m_playerICM.getActions()[ska::InputAction::DeleteChar];
	if (isDeleting) {
		KeyEvent ke(KeyEventType::KEY_DOWN, L"", SDL_SCANCODE_BACKSPACE);
		KeyObservable::notifyObservers(ke);
	}

	if (!textTyped.empty()) {

		KeyEvent ke(KeyEventType::TEXT_TYPED, textTyped, -1);
		KeyObservable::notifyObservers(ke);

		//TODO Focus, Blur
		//TODO optionnel : Key down et Key up events ?

		/*HoverEvent hove(MouseEventType::MOUSE_OVER, lastMousePos, mousePos);
		HoverObservable::notifyObservers(hove);

		if (m_hovered != nullptr) {
			for (auto it = m_hovered; it != hove.getTarget() && it != nullptr; it = it->getParent()) {
				if (hove.getTarget() == nullptr || !it->isAParent(*hove.getTarget())) {
					HoverEvent heOut(MouseEventType::MOUSE_OUT, it->getAbsolutePosition(), mousePos);
					it->mouseHover(heOut);
				}
			}
		}
		m_hovered = hove.getTarget();
		*/

	}
}

void ska::GUI::refreshMouse() {
	const ska::InputActionContainer& in = m_playerICM.getActions();
	const auto& moveWindow = m_playerICM.getToggles()[ska::InputToggle::MoveWindow];
	const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
	const ska::InputRange& lastMousePos = m_playerICM.getRanges()[ska::InputRangeType::LastMousePos];


	if (mousePos != lastMousePos) {
		/* Toujours utiliser lastMousePos et non pas mousePos pour les hover :
		*  on considère mousePos comme la prochaine position de la souris en terme d'évènements.
		*  On a donc un retard d'une frame sur tous les évènements déclenchés, mais cela permet de pouvoir réagir
		*  aux changements brusques de position de souris */
		HoverEvent he(MouseEventType::MOUSE_ENTER, lastMousePos, mousePos);
		HoverObservable::notifyObservers(he);

		HoverEvent hove(MouseEventType::MOUSE_OVER, lastMousePos, mousePos);
		HoverObservable::notifyObservers(hove);

		if (m_hovered != nullptr) {
			for (auto it = m_hovered; it != hove.getTarget() && it != nullptr; it = it->getParent()) {
				if (hove.getTarget() == nullptr || !it->isAParent(*hove.getTarget())) {
					HoverEvent heOut(MouseEventType::MOUSE_OUT, it->getAbsolutePosition(), mousePos);
					it->directNotify(heOut);
				}
			}

		}
		m_hovered = hove.getTarget();


	}

	if (in[ska::InputAction::LClic]) {
		FocusEvent fbe(MouseEventType::MOUSE_BLUR);
		if (m_lastFocused != nullptr) {
			m_lastFocused->directNotify(fbe);
		}

		if (fbe.getTarget() != nullptr) {
			//TODO : add map context
			m_playerICM.disableContext(EnumContextManager::MAP, false);
		}

		ska::Point<int> pMp (mousePos);
		ClickEvent ce(MouseEventType::MOUSE_CLICK, pMp);
		ClickObservable::notifyObservers(ce);
		m_clicked = ce.getTarget();
	
		if (m_clicked != nullptr) {
			FocusEvent fe(MouseEventType::MOUSE_FOCUS);
			m_clicked->directNotify(fe);
			if(fe.getTarget() != nullptr) {
				//TODO : remove map context
				m_playerICM.disableContext(EnumContextManager::MAP, true);
			}
			
			m_lastFocused = m_clicked;
		}
	}


	if (m_clicked != nullptr && !moveWindow) {
        ska::Point<int> pMp (mousePos);
		ClickEvent ce(MouseEventType::MOUSE_RELEASE, pMp);
		ce.setTarget(m_clicked);
		m_clicked = nullptr;
		ClickObservable::notifyObservers(ce);
	}
}

void ska::GUI::refresh() {
	if (m_hide) {
		return;
	}

	refreshMouse();
	refreshKeyboard();

	//update();

	//Time-based events
	for (auto it = std::begin(m_dynamicWindows); it != m_dynamicWindows.end(); )  {
		if(!(*it)->refresh()) {
			it = m_dynamicWindows.erase(it);
		} else {
			it++;
		}
	}

// 	if (in[ska::InputAction::RClic]) {
// 		setClickMenu();
// 	}

// 	if (hideHint == m_buttonScroll.size()) {
// 		m_mouseCursor.hideHint(true);
// 	}
//
// 	m_mouseCursor.update();

}

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


void ska::GUI::update() {
	m_hide = false;

	std::vector<std::vector<std::unique_ptr<DynamicWindowIG<>>>::iterator> toDelete;
	unsigned int index = 0;
	/*for (auto w = m_extraWindows.begin(); w != m_extraWindows.end(); w++) {
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
	}*/
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

void ska::GUI::windowSorter(Widget* tthis, ClickEvent& e) {
	if (e.getState() == MouseEventType::MOUSE_CLICK) {
		auto firstWidget = m_wMaster.backWidget();
		if (tthis != firstWidget) {
			auto firstPriority = firstWidget->getPriority();
			auto priority = tthis->getPriority();
			firstWidget->setPriority(priority);
			tthis->setPriority(firstPriority);
			tthis->focus(true);
			firstWidget->focus(false);
			m_wMaster.resort();
		}
	}
}

// int ska::GUI::getRefreshPNJWindowCount() {
//     return m_refreshCount;
// }
//
// void ska::GUI::setRefreshPNJWindowCount(int x) {
//     m_refreshCount = x;
// }
