#pragma once
#include "../Components/WidgetPanel.h"
#include "Graphic/Texture.h"


namespace ska{
	class Texture;

	template <class ...HL>
	class WindowIG :
		public WidgetPanel<HL...> {
	public:
		WindowIG(Widget& parent, const Rectangle& box, const std::string& styleName) :
			WidgetPanel<HL...>(parent),
			m_drawStyle(!styleName.empty()),
			m_menuTiles(styleName + ".png") {

			this->move(box);
			this->setWidth(box.w);
			this->setHeight(box.h);
		}

		WindowIG(const Rectangle& box, const std::string& styleName) :
			m_drawStyle(!styleName.empty()),
			m_menuTiles(styleName + ".png") {

			this->move(box);
			this->setWidth(box.w);
			this->setHeight(box.h);
		}

		void display() const override {
			if (!this->isVisible()) {
				return;
			}

            /* Draw the window using menu tiles */
			if (m_drawStyle) {
                Rectangle backgroundTileClip = { 0, 0, TAILLEBLOCFENETRE, TAILLEBLOCFENETRE };
                //Rectangle maxSizeMenuSprite = { 0, 0, m_menuTiles.getWidth(), m_menuTiles.getHeight() };
                const auto& box = this->getBox();
                const auto& pos = this->getAbsolutePosition();

				for (auto i = 0; i < box.w; i += backgroundTileClip.w) {
					for (auto j = 0; j < box.h; j += backgroundTileClip.h) {
						const Point<int> backgroundTilePos(pos.x + i, pos.y + j);
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
