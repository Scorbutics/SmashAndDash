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
	addHoverHandler(std::bind(&ska::WindowIG::onMouseHover, this, std::placeholders::_1));
	addClickHandler(std::bind(&ska::WindowIG::onClick, this, std::placeholders::_1));
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
	addHoverHandler(std::bind(&ska::WindowIG::onMouseHover, this, std::placeholders::_1));
	addClickHandler(std::bind(&ska::WindowIG::onClick, this, std::placeholders::_1));
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

bool ska::WindowIG::onClick(ska::ClickEvent& e) {
	for(auto& w : m_widgets) {
		if(!w->click(e)) {
			return false;
		}
	}
	//e.setTarget(this);
	return true;
}

bool ska::WindowIG::onMouseHover(ska::HoverEvent& e) {
	for (auto& w : m_widgets) {
		if(!w->mouseHover(e)) {
			break;
		}
	}

	return false;
}

void ska::WindowIG::refresh() {
	if (m_speed < 1.0) {
		m_speed = 0.0;
		return;
	}
	
	const auto& pos = getRelativePosition();

	bool xdone = false;
	bool ydone = false;
	auto nextPos = pos;
	unsigned int distx = (m_slope.x != 0.0) ? (m_destinationPos.x - pos.x) * (m_destinationPos.x - pos.x) : 0;
	xdone = distx == 0;
	if (m_slope.x*m_slope.x < distx) {
		nextPos.x = m_destinationPos.x;
		m_slope.x = 0;
	} else {
		nextPos.x += m_slope.x;
	}

	unsigned int disty = (m_slope.y != 0.0) ? (m_destinationPos.y - pos.y) * (m_destinationPos.y - pos.y) : 0;
	ydone = disty == 0;
	if (m_slope.y*m_slope.y < disty) {
		nextPos.y = m_destinationPos.y;
		m_slope.y = 0;
	} else {
		nextPos.y += m_slope.y;
	}

	if(xdone && ydone) {
		m_speed = 0.0;
	}

	move(nextPos);
}

void ska::WindowIG::scrollTo(const ska::Point<int>& relativeTargetPos, unsigned int steps) {
	m_destinationPos = relativeTargetPos;
	m_speed = steps == 0 ? 1.0 : (1.0 / steps);
	
	const auto& pos = getRelativePosition();
	m_slope.y = (m_destinationPos.y - pos.y) * m_speed;
	m_slope.x = (m_destinationPos.x - pos.x) * m_speed;

	if (ska::NumberUtils::isLowValue(m_slope.x, 1.0) && m_destinationPos.x != pos.x) {
		m_slope.x = ska::NumberUtils::calculateSlopeSign(m_destinationPos.y, pos.y);
	}

	if (ska::NumberUtils::isLowValue(m_slope.y, 1.0) && m_destinationPos.y != pos.y) {
		m_slope.y = ska::NumberUtils::calculateSlopeSign(m_destinationPos.x, pos.x);
	}
	
}

ska::WindowIG::~WindowIG() {
	if (m_guiObservable != nullptr) {
		m_guiObservable->HoverObservable::removeObserver(*this);
		m_guiObservable->ClickObservable::removeObserver(*this);
	}
}
