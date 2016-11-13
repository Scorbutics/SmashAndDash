// #include "WindowIG.h"
// 
// #include "GUI.h"
// 
// ska::WindowIG::WindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle) :
// Button(parent),
// 
// 
// ska::WindowIG::WindowIG(MouseObservable& guiObservable, KeyboardObservable& keyObservable, const ska::Rectangle& box, bool drawStyle) :
// Button(),
// 
// }
// 
// void ska::WindowIG::initHandlers() {
// 	addHeadHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
// 		for (auto& w : m_hoverWidgets) {
// 			w->mouseHover(e);
// 			if (e.stopped() == StopType::STOP_WIDGET) {
// 				return;
// 			}
// 		}
// 	});
// 
// 	addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
// 		for (auto& w : m_clickWidgets) {
// 			w->click(e);
// 			if (e.stopped() == StopType::STOP_WIDGET) {
// 				return;
// 			}
// 		}
// 	});
// 
// 
// 	addHeadHandler<ska::KeyEventListener>([&](ska::Widget* tthis, ska::KeyEvent& e) {
// 		for (auto& w : m_keyWidgets) {
// 			w->keyEvent(e);
// 			if (e.stopped() == StopType::STOP_WIDGET) {
// 				return;
// 			}
// 		}
// 	});
// }
// 
// void ska::WindowIG::display() const {
// 	if(!isVisible()) {
// 		return;
// 	}
// 	ska::Rectangle backgroundTileClip = { 0, 0, TAILLEBLOCFENETRE, TAILLEBLOCFENETRE };
// 	ska::Rectangle maxSizeMenuSprite = { 0, 0, m_menuTiles.getWidth(), m_menuTiles.getHeight()};
// 	const auto& box = getBox();
// 	const auto& pos = getAbsolutePosition();
// 
// 	/* Draw the window using menu tiles */
// 	if (m_drawStyle) {
// 		for (int i = 0; i < box.w; i += backgroundTileClip.w) {
// 			for (int j = 0; j < box.h; j += backgroundTileClip.h) {
// 				const ska::Point<int> backgroundTilePos(pos.x + i, pos.y + j);
// 				const int xcondition1 = (i / backgroundTileClip.w >= 1);
// 				const int xcondition2 = ((box.w != backgroundTileClip.w) && (i / (box.w - backgroundTileClip.w) >= 1));
// 
// 				const int ycondition1 = (j / backgroundTileClip.h >= 1);
// 				const int ycondition2 = ((box.h != backgroundTileClip.h) && (j / (box.h - backgroundTileClip.h) >= 1));
// 
// 				backgroundTileClip.x = xcondition1 * backgroundTileClip.w + xcondition2 * backgroundTileClip.w;
// 				backgroundTileClip.y = ycondition1 * backgroundTileClip.h + ycondition2 * backgroundTileClip.h;
// 
// 				m_menuTiles.render(backgroundTilePos.x, backgroundTilePos.y, &backgroundTileClip);
// 			}
// 		}
// 	}
// 
// 	for (const auto& area : m_widgets){
// 		area->display();
// 	}
// 
// }
// 
// ska::WindowIG::~WindowIG() {
// 	if (m_guiObservable != nullptr) {
// 		m_guiObservable->HoverObservable::removeObserver(*this);
// 		m_guiObservable->ClickObservable::removeObserver(*this);
// 	}
// 
// 	if (m_keyObservable != nullptr) {
// 		m_keyObservable->removeObserver(*this);
// 	}
// }
