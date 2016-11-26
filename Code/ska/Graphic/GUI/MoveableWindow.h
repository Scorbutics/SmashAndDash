#pragma once
#include "DynamicWindowIG.h"
#include "Components/Button.h"
#include "Components/ButtonQuit.h"
#include "Components/KeyObservable.h"

namespace ska {

	template <class ...HL>
	class MoveableWindow : public DynamicWindowIG<HL...> {
	public:
		MoveableWindow(MouseObservable& guiObservable, KeyObservable& keyboardObservable, const ska::Rectangle& box, bool drawStyle) :
			ska::DynamicWindowIG<HL...>(&guiObservable, &keyboardObservable, box, drawStyle) {
			initHandlers();
		}

		MoveableWindow(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle) :
			ska::DynamicWindowIG<HL...>(parent, box, drawStyle) {
			initHandlers();
		}

	private:
		void initHandlers() {
			const auto& clip = ska::Rectangle{ 0, 0, this->getBox().w, TAILLEBLOCFENETRE / 2 };
			auto button = std::unique_ptr<ska::Button>(new Button(*this, ska::Point<int>(), ska::Button::MENU_DEFAULT_THEME_PATH + "button", &clip, [&](ska::Widget* tthis, ska::ClickEvent& e) {
				if (e.getState() == ska::MouseEventType::MOUSE_CLICK) {
					m_moving = true;
					m_offsetWindowOrigin = e.getPosition(*tthis);
				}
				else if (e.getState() == ska::MouseEventType::MOUSE_RELEASE) {
					m_moving = false;
				}
			}));
			button->setWidth(this->getBox().w - TAILLEBLOCFENETRE / 2);
			button->setHeight(TAILLEBLOCFENETRE / 2);
			button->addHandler<HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
				if (m_moving && e.getState() != ska::MouseEventType::MOUSE_ENTER) {
					const auto& clickAbsPos = e.getMousePosition();
					const auto& newPos = clickAbsPos - m_offsetWindowOrigin;
					this->move(newPos);
				}
			});

			auto buttonQuit = std::unique_ptr<ska::ButtonQuit>(new ButtonQuit(*this, ska::Point<int>(this->getBox().w - TAILLEBLOCFENETRE / 2, 0), ska::Button::MENU_DEFAULT_THEME_PATH + "close_button"));

			
			this->addWidget(button);
			this->addWidget(buttonQuit);
		}

		bool m_moving;
		ska::Point<int> m_offsetWindowOrigin;
	};
}
