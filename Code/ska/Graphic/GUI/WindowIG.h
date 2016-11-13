#pragma once
#include "../../Utils/SkaConstants.h"
#include "../../Utils/RectangleUtils.h"
#include "./Components/WidgetPanel.h"
#include "../Texture.h"

#include "./Components/Button.h"

namespace ska{
	
	template <class ...HL>
	class WindowIG : 
		public WidgetPanel<HL...> {
	public:
		WindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle) : 
			WidgetPanel<HL...>(parent),
			m_menuTiles(ska::Button::MENU_DEFAULT_THEME_PATH + "menu.png"),
			m_drawStyle(drawStyle) {

			move(box);
			setWidth(box.w);
			setHeight(box.h);
		}

		WindowIG(const ska::Rectangle& box, bool drawStyle) :
			m_menuTiles(ska::Button::MENU_DEFAULT_THEME_PATH + "menu.png"),
			m_drawStyle(drawStyle) {

			move(box);
			setWidth(box.w);
			setHeight(box.h);
		}

		void display() const override {
			if (!isVisible()) {
				return;
			}
			ska::Rectangle backgroundTileClip = { 0, 0, TAILLEBLOCFENETRE, TAILLEBLOCFENETRE };
			ska::Rectangle maxSizeMenuSprite = { 0, 0, m_menuTiles.getWidth(), m_menuTiles.getHeight() };
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
			WidgetPanel<HL...>::display();
		}
		
		virtual ~WindowIG() = default;
	
	private:
		bool m_drawStyle;
		Texture m_menuTiles;
	};

}