#include "Core/Window.h"
#include "Inputs/InputContextManager.h"
#include "GUI.h"
#include "Components/Concrete/Button.h"
#include "Windows/GUIScrollButtonWindowIG.h"
#include "Events/FocusEvent.h"
#include "Data/Events/GUIEvent.h"

#define SCROLL_BUTTON_SPEED 3

ska::GUI::GUI(ska::GameEventDispatcher& ged, const ska::BaseWindow& w, ska::InputContextManager& playerICM) :
    ska::Observer<GUIEvent>(std::bind(&ska::GUI::onGUIEvent, this, std::placeholders::_1)),
    m_mouseCursor(Button::MENU_DEFAULT_THEME_PATH + "mouse_cursor"),
    m_window(w),
    m_playerICM(playerICM),
    m_ged(ged),
    m_hovered(nullptr),
    m_clicked(nullptr),
    m_lastFocused(nullptr),
    m_wMaster(this, this, this, Rectangle{ 0, 0, static_cast<int>(w.getWidth()), static_cast<int>(w.getHeight()) }, "") {

    m_wFocusable = &m_wMaster.addWidget<TimeScrollableWindowIG<KeyEventListener>>(Rectangle{ 0, 0, static_cast<int>(w.getWidth()), static_cast<int>(w.getHeight()) }, "");
    DrawableFixedPriority::setPriority(std::numeric_limits<int>().max());

	//m_wFocusable->setPriority(1);
	m_hide = false;

	m_ged.ska::Observable<GUIEvent>::addObserver(*this);
}

ska::GUI::~GUI() {
    m_ged.ska::Observable<GUIEvent>::removeObserver(*this);
}

unsigned int ska::GUI::getMaxHeight() const{
    return m_window.getHeight();
}

unsigned int ska::GUI::getMaxWidth() const{
    return m_window.getWidth();
}

void ska::GUI::display() const {
	if (m_hide) {
		return;
	}

	m_wMaster.display();

	for (auto& w : m_topWindowWidgets) {
		w->display();
	}

	m_mouseCursor.display();
}

void ska::GUI::refreshKeyboard() {
	const auto& textTyped = m_playerICM.getTextInput();
    //TODO réécrire
	const auto& isDeleting = m_playerICM.getActions()[DeleteChar];
	if (isDeleting) {
		KeyEvent ke(KEY_DOWN, L"", SDL_SCANCODE_BACKSPACE);
		KeyObservable::notifyObservers(ke);
	}

	const auto& isValidating = m_playerICM.getActions()[DoAction];
	if (isValidating) {
		KeyEvent ke(KEY_DOWN, L"", SDL_SCANCODE_RETURN);
		KeyObservable::notifyObservers(ke);
	}

	if (!textTyped.empty()) {
		KeyEvent ke(TEXT_TYPED, textTyped, -1);
		KeyObservable::notifyObservers(ke);
	}
}

void ska::GUI::refreshMouse() {
    const auto& in = m_playerICM.getActions();
	const auto& moveWindow = m_playerICM.getToggles()[MoveWindow];
	const auto& mousePos = m_playerICM.getRanges()[MousePos];
	const auto& lastMousePos = m_playerICM.getRanges()[LastMousePos];

    if (m_lastLastMousePos != lastMousePos || lastMousePos != mousePos) {
		m_mouseCursor.move(mousePos);

		/* Toujours utiliser lastMousePos et non pas mousePos pour les hover :
		*  on considère mousePos comme la prochaine position de la souris en terme d'évènements.
		*  On a donc un retard d'une frame sur tous les évènements déclenchés, mais cela permet de pouvoir réagir
		*  aux changements brusques de position de souris */
		HoverEvent he(MOUSE_ENTER, lastMousePos, mousePos, mousePos - lastMousePos);
		HoverObservable::notifyObservers(he);

		HoverEvent hove(MOUSE_OVER, lastMousePos, mousePos, mousePos - lastMousePos);
		HoverObservable::notifyObservers(hove);

		if (m_hovered != nullptr) {
			for (auto it = m_hovered; it != hove.getTarget() && it != nullptr; it = it->getParent()) {
				if (hove.getTarget() == nullptr || !it->isAParent(*hove.getTarget())) {
					HoverEvent heOut(MOUSE_OUT, it->getAbsolutePosition(), mousePos, mousePos - lastMousePos);
					it->directNotify(heOut);
				}
			}

		}
		m_hovered = hove.getTarget();

	}
    m_lastLastMousePos = lastMousePos;

	auto click = in[LClic];
	if (click) {
		Point<int> pMp (mousePos);
		ClickEvent ce(MOUSE_CLICK, pMp);
		ClickObservable::notifyObservers(ce);
		m_clicked = ce.getTarget();

		auto lastFocused = m_lastFocused;
		if (m_clicked != nullptr) {
			FocusEvent fe(m_clicked, MOUSE_FOCUS);
			m_clicked->directNotify(fe);
			if(fe.getTarget() != nullptr) {
				m_playerICM.disableContext(CONTEXT_MAP, true);
			}

			m_lastFocused = m_clicked;
		}

		FocusEvent fbe(m_clicked, MOUSE_BLUR);
		if (lastFocused != nullptr && lastFocused != m_clicked) {
			lastFocused->directNotify(fbe);
		}

		if (fbe.getTarget() != nullptr) {
			m_playerICM.disableContext(CONTEXT_MAP, false);
		}
	}


	if (m_clicked != nullptr && !moveWindow) {
        Point<int> pMp (mousePos);
		ClickEvent ce(MOUSE_RELEASE, pMp);
		ce.setTarget(m_clicked);
		m_clicked = nullptr;
		ClickObservable::notifyObservers(ce);
	}


}

void ska::GUI::refresh() {
	if (m_hide) {
		return;
	}

	for(auto& wName : m_windowsToDelete) {
		auto windowRemoved = (removeWindow(wName));
		if(windowRemoved == m_hovered) {
			m_hovered = nullptr;
		} else if (windowRemoved == m_clicked) {
			m_clicked = nullptr;
		}
	}
    m_windowsToDelete.clear();

	refreshMouse();
	refreshKeyboard();

	//Time-based events
	TimeEvent te;
	TimeObservable::notifyObservers(te);

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
        m_pokeInfoWindow->addButtonClose("." FILE_SEPARATOR "Menu" FILE_SEPARATOR "close_button.png", "." FILE_SEPARATOR "Menu" FILE_SEPARATOR "close_button_active.png", buf);
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
			ss << "." FILE_SEPARATOR "Facesets" FILE_SEPARATOR "" << id << ".png";
		} else {
			ss << "." FILE_SEPARATOR "Facesets" FILE_SEPARATOR "" << -id << ".png";
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
        m_pnjInfoWindow->addButtonClose("." FILE_SEPARATOR "Menu" FILE_SEPARATOR "close_button.png", "." FILE_SEPARATOR "Menu" FILE_SEPARATOR "close_button_active.png", buf);
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
			ss << "." FILE_SEPARATOR "Facesets" FILE_SEPARATOR "" << id << ".png";
		} else {
			ss << "." FILE_SEPARATOR "Facesets" FILE_SEPARATOR "" << -id << ".png";
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
	if (e.getState() == MOUSE_CLICK) {
		pushWindowToFront(tthis);
	}
}

bool ska::GUI::onGUIEvent(GUIEvent& ge) {
    if(ge.type == ska::GUIEventType::REMOVE_WINDOW) {
        m_windowsToDelete.push_back(ge.windowName);
    }
    return false;
}

ska::Widget* ska::GUI::frontWindow() {
    return m_wFocusable->backWidget();
}

void ska::GUI::pushWindowToFront(Widget* w) {
	auto firstWidget = frontWindow();
	if (w != firstWidget) {
		auto firstPriority = firstWidget->getPriority();
		auto priority = w->getPriority();
		firstWidget->setPriority(priority);
		w->setPriority(firstPriority);
		w->focus(true);
		firstWidget->focus(false);
		m_wFocusable->resort();
	}
}

ska::Widget* ska::GUI::addTopWidget(std::unique_ptr<Widget>& w) {
	m_topWindowWidgets.push_back(move(w));
	return m_topWindowWidgets.back().get();
}
