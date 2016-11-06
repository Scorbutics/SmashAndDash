#include "WindowIG.h"
#include "../../Utils/SkaConstants.h"
#include "../../Utils/RectangleUtils.h"
#include "./Components/ClickEvent.h"
#include "GUI.h"

ska::WindowIG::WindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle) :
Button(parent),
m_menuTiles(ska::Button::MENU_DEFAULT_THEME_PATH + "menu.png"),
m_drawStyle(drawStyle),
m_guiObservable(nullptr),
m_keyObservable(nullptr) {
	move(box);
	setWidth(box.w);
	setHeight(box.h);
	initHandlers();
}


bool ska::WindowIG::isAffectedBy(const ska::KeyEvent& e) const {
	return true;
}

ska::WindowIG::WindowIG(MouseObservable& guiObservable, KeyboardObservable& keyObservable, const ska::Rectangle& box, bool drawStyle) :
Button(),
	m_menuTiles(ska::Button::MENU_DEFAULT_THEME_PATH + "menu.png"),
	m_drawStyle(drawStyle),
	m_guiObservable(&guiObservable),
	m_keyObservable(&keyObservable) {
	move(box);
	setWidth(box.w);
	setHeight(box.h);
	m_guiObservable->HoverObservable::addObserver(*this);
	m_guiObservable->ClickObservable::addObserver(*this);
	m_keyObservable->addObserver(*this);
	initHandlers();
}

void ska::WindowIG::initHandlers() {
	addHeadHoverHandler([&](ska::Widget* tthis, ska::HoverEvent& e) {
		for (auto& w : m_widgets) {
			w->mouseHover(e);
			if (e.stopped() == StopType::STOP_WIDGET) {
				return;
			}
		}
	});

	addHeadClickHandler([&](ska::Widget* tthis, ska::ClickEvent& e) {
		for (auto& w : m_widgets) {
			w->click(e);
			if (e.stopped() == StopType::STOP_WIDGET) {
				return;
			}
		}
	});


	addHeadKeyHandler([&](ska::Widget* tthis, ska::KeyEvent& e) {
		for (auto& w : m_widgets) {
			w->keyEvent(e);
			if (e.stopped() == StopType::STOP_WIDGET) {
				return;
			}
		}
	});
}

void ska::WindowIG::display() const {
	if(!isVisible()) {
		return;
	}
	ska::Rectangle backgroundTileClip = { 0, 0, TAILLEBLOCFENETRE, TAILLEBLOCFENETRE };
	ska::Rectangle maxSizeMenuSprite = { 0, 0, m_menuTiles.getWidth(), m_menuTiles.getHeight()};
	const auto& box = getBox();
	const auto& pos = getAbsolutePosition();

	/* Draw the window using menu tiles */
	if (m_drawStyle) {
		for (int i = 0; i < box.w; i += backgroundTileClip.w) {
			for (int j = 0; j < box.h; j += backgroundTileClip.h) {
				const ska::Point<int> backgroundTilePos(pos.x + i, pos.y + j);
				const int xcondition1 = (i / backgroundTileClip.w >= 1);
				const int xcondition2 = ((box.w != backgroundTileClip.w) && (i / (box.w - backgroundTileClip.w) >= 1));

				const int ycondition1 = (j / backgroundTileClip.h >= 1);
				const int ycondition2 = ((box.h != backgroundTileClip.h) && (j / (box.h - backgroundTileClip.h) >= 1));

				backgroundTileClip.x = xcondition1 * backgroundTileClip.w + xcondition2 * backgroundTileClip.w;
				backgroundTileClip.y = ycondition1 * backgroundTileClip.h + ycondition2 * backgroundTileClip.h;

				m_menuTiles.render(backgroundTilePos.x, backgroundTilePos.y, &backgroundTileClip);
			}
		}
	}

	for (const auto& area : m_widgets){
		area->display();
	}

}

ska::WindowIG::~WindowIG() {
	if (m_guiObservable != nullptr) {
		m_guiObservable->HoverObservable::removeObserver(*this);
		m_guiObservable->ClickObservable::removeObserver(*this);
	}

	if (m_keyObservable != nullptr) {
		m_keyObservable->removeObserver(*this);
	}
}
