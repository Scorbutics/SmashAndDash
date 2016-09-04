#include "WindowIG.h"
#include "../../Utils/SkaConstants.h"
#include "./Components/ClickEvent.h"
#include "GUI.h"

ska::WindowIG::WindowIG(MouseObservable& guiObservable, ska::Widget& parent, const ska::Rectangle& box, bool drawStyle) :
	Widget(parent),
	MouseObserver(std::bind(&ska::WindowIG::mouseHover, this, std::placeholders::_1), std::bind(&ska::WindowIG::click, this, std::placeholders::_1)),
	m_menuTiles("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png"),
	m_drawStyle(drawStyle),
	m_guiObservable(guiObservable) {
	move(box);
	setWidth(box.w);
	setHeight(box.h);
	m_guiObservable.HoverObservable::addObserver(*this);
	m_guiObservable.ClickObservable::addObserver(*this);
}

ska::WindowIG::WindowIG(MouseObservable& guiObservable, const ska::Rectangle& box, bool drawStyle) :
	Widget(),
	MouseObserver(std::bind(&ska::WindowIG::mouseHover, this, std::placeholders::_1), std::bind(&ska::WindowIG::click, this, std::placeholders::_1)),
	m_menuTiles("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png"),
	m_drawStyle(drawStyle),
	m_guiObservable(guiObservable) {
	move(box);
	setWidth(box.w);
	setHeight(box.h);
	m_guiObservable.HoverObservable::addObserver(*this);
	m_guiObservable.ClickObservable::addObserver(*this);
}

void ska::WindowIG::display() const {
	ska::Rectangle backgroundTileClip = { 0, 0, TAILLEBLOCFENETRE, TAILLEBLOCFENETRE };
	const auto& box = getBox();
	const auto& pos = getAbsolutePosition();

	/* Draw the window using menu tiles */
	if (m_drawStyle) {
		for (int i = 0; i < box.w; i += TAILLEBLOCFENETRE) {
			for (int j = 0; j < box.h; j += TAILLEBLOCFENETRE) {
				const ska::Point<int> backgroundTilePos(pos.x + i, pos.y + j);

				backgroundTileClip.x = box.w == TAILLEBLOCFENETRE ? 0 : ((((float)i) / (box.w - TAILLEBLOCFENETRE)) * 2 * TAILLEBLOCFENETRE);
				backgroundTileClip.y = box.h == TAILLEBLOCFENETRE ? 0 : ((((float)j) / (box.h - TAILLEBLOCFENETRE)) * 2 * TAILLEBLOCFENETRE);

				m_menuTiles.render(backgroundTilePos.x, backgroundTilePos.y, &backgroundTileClip);
			}
		}
	}

	for (const auto& area : m_widgets){
		area->display();
	}

}

bool ska::WindowIG::click(ska::ClickEvent& e) {
	const auto& affect = e.isOn(*this);
	if(!affect) {
		return true;
	}

	ClickObservable::notifyObservers(e);

	return false;
}

bool ska::WindowIG::mouseHover(ska::HoverEvent& e) {
	const auto& affect = e.isOn(*this);
	if (!affect) {
		return true;
	}

	HoverObservable::notifyObservers(e);

	return false;
}

ska::WindowIG::~WindowIG() {
	m_guiObservable.HoverObservable::removeObserver(*this);
	m_guiObservable.ClickObservable::removeObserver(*this);
}
