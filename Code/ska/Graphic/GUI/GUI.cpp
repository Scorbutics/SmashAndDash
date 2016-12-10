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
m_wMaster(this, this, ska::Rectangle{ 0, 0, w.getWidth(), w.getHeight() }, "") {

	m_wAction = new DynamicWindowIG<>(m_wMaster, ska::Rectangle{ 0, 0, 13 * TAILLEBLOCFENETRE, 2 * TAILLEBLOCFENETRE }, "");
    m_hide = false;

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

	//m_wAction->clear();
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

	auto firstButton = std::unique_ptr<GUIScrollButtonWindowIG>(new GUIScrollButtonWindowIG(*m_wAction, buf, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"));
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
	auto secondButton = std::unique_ptr<GUIScrollButtonWindowIG>(new GUIScrollButtonWindowIG(*m_wAction, buf, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"));
	secondButton->addHandler<HoverEventListener>(scrollButtonLambda);
	m_wAction->addWidget(secondButton);
	//m_buttonList[1]->name("Pokédex");
	//m_buttonList[1]->setActionClic("pokedex");

    buf.x += 5*TAILLEBLOCFENETRE/2;
	//m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"bag.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
	auto thirdButton = std::unique_ptr<GUIScrollButtonWindowIG>(new GUIScrollButtonWindowIG(*m_wAction, buf, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"));
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
	auto fourthButton = std::unique_ptr<GUIScrollButtonWindowIG>(new GUIScrollButtonWindowIG(*m_wAction, buf, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"));
	fourthButton->addHandler<HoverEventListener>(scrollButtonLambda);
	m_wAction->addWidget(fourthButton);
    //m_buttonList[3]->setActionClic("trainer_card");
	//m_buttonList[3]->name("Carte dresseur");

    buf.x += 5*TAILLEBLOCFENETRE/2;
	//m_buttonList.push_back(DialogMenuPtr(new DialogMenu("", "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"tape.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"toolsmenu.png", buf, 22, false)));
	auto fifthButton = std::unique_ptr<GUIScrollButtonWindowIG>(new GUIScrollButtonWindowIG(*m_wAction, buf, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"));
	fifthButton->addHandler<HoverEventListener>(scrollButtonLambda);
	std::unique_ptr<ButtonSprite> bsFifth = std::unique_ptr<ButtonSprite>(new ButtonSprite(*fifthButton, Point<int>(1, 1), "", 104, [&](ska::Widget* tthis, const ska::ClickEvent& e) {
		if (e.getState() == ska::MouseEventType::MOUSE_CLICK) {
			auto w = m_windowAnnuary["settings"];
			w->show(!w->isVisible());
		}
	}));
	fifthButton->addWidget(std::move(bsFifth));
	m_wAction->addWidget(fifthButton);
}

void ska::GUI::display() const {
	if (m_hide) {
		return;
	}

	m_wMaster.display();
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
			m_playerICM.disableContext(EnumContextManager::CONTEXT_MAP, false);
		}

		ska::Point<int> pMp (mousePos);
		ClickEvent ce(MouseEventType::MOUSE_CLICK, pMp);
		ClickObservable::notifyObservers(ce);
		m_clicked = ce.getTarget();
	
		if (m_clicked != nullptr) {
			FocusEvent fe(MouseEventType::MOUSE_FOCUS);
			m_clicked->directNotify(fe);
			if(fe.getTarget() != nullptr) {
				m_playerICM.disableContext(EnumContextManager::CONTEXT_MAP, true);
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

	//Time-based events
	for (auto it = std::begin(m_dynamicWindows); it != m_dynamicWindows.end(); )  {
		if(!(*it)->refresh()) {
			it = m_dynamicWindows.erase(it);
		} else {
			it++;
		}
	}

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

void ska::GUI::hide(bool x) {
    m_hide = x;
}

bool ska::GUI::isVisible() const {
    return !m_hide;
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
