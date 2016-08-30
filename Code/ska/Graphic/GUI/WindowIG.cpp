#include "WindowIG.h"
#include "../../Utils/SkaConstants.h"

ska::WindowIG::WindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle) :
	Widget(parent),
	m_menuTiles("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png"),
	m_drawStyle(drawStyle) {
	move(box);
	setWidth(box.w);
	setHeight(box.h);

}

ska::WindowIG::WindowIG(const ska::Rectangle& box, bool drawStyle) :
	Widget(),
	m_menuTiles("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png"),
	m_drawStyle(drawStyle) {
	move(box);
	setWidth(box.w);
	setHeight(box.h);
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
