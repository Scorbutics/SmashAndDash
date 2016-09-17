#include "WindowIG.h"
#include "../../Utils/SkaConstants.h"
#include "../../Utils/RectangleUtils.h"
#include "./Components/ClickEvent.h"
#include "GUI.h"

ska::WindowIG::WindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle) :
Button(parent),
m_menuTiles("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png"),
m_drawStyle(drawStyle),
m_guiObservable(nullptr) {
	move(box);
	setWidth(box.w);
	setHeight(box.h);
	addHeadHoverHandler(std::bind(&ska::WindowIG::onMouseHover, this, this, std::placeholders::_2));
	addHeadClickHandler(std::bind(&ska::WindowIG::onClick, this, this, std::placeholders::_2));
}


ska::WindowIG::WindowIG(MouseObservable& guiObservable, const ska::Rectangle& box, bool drawStyle) :
Button(),
	m_menuTiles("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png"),
	m_drawStyle(drawStyle),
	m_guiObservable(&guiObservable) {
	move(box);
	setWidth(box.w);
	setHeight(box.h);
	m_guiObservable->HoverObservable::addObserver(*this);
	m_guiObservable->ClickObservable::addObserver(*this);
	addHeadHoverHandler(std::bind(&ska::WindowIG::onMouseHover, this, this, std::placeholders::_2));
	addHeadClickHandler(std::bind(&ska::WindowIG::onClick, this, this, std::placeholders::_2));
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

bool ska::WindowIG::onClick(ska::Widget* tthis, ska::ClickEvent& e) {
	for(auto& w : m_widgets) {
		if(!w->click(e)) {
			return false;
		}
	}
	//e.setTarget(this);
	return true;
}

bool ska::WindowIG::onMouseHover(ska::Widget* tthis, ska::HoverEvent& e) {
	for (auto& w : m_widgets) {
		w->mouseHover(e);
	}
	
	return false;
}

ska::WindowIG::~WindowIG() {
	if (m_guiObservable != nullptr) {
		m_guiObservable->HoverObservable::removeObserver(*this);
		m_guiObservable->ClickObservable::removeObserver(*this);
	}
}
