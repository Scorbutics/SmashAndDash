#pragma once
#include "DynamicWindowIG.h"
#include "../Components/Concrete/Button.h"
#include "../Components/Concrete/ButtonQuit.h"
#include "../Events/HoverEvent.h"

namespace ska {

	template <class ...HL>
	class MoveableWindow : public DynamicWindowIG<HL...> {
	public:
		MoveableWindow(MouseObservable& guiObservable, KeyObservable& keyboardObservable, const Rectangle& box, const std::string& styleName) :
			DynamicWindowIG<HL...>(&guiObservable, &keyboardObservable, box, styleName),
			m_moving(false) {
			initHandlers();
		}

		MoveableWindow(Widget& parent, const Rectangle& box, const std::string& styleName) :
			DynamicWindowIG<HL...>(parent, box, styleName),
			m_moving(false) {
			initHandlers();
		}

	private:
		void initHandlers() {
			const auto& clip = Rectangle{ 0, 0, this->getBox().w, TAILLEBLOCFENETRE / 2 };
			auto& button = this->template addWidget<Button>(Point<int>(), Button::MENU_DEFAULT_THEME_PATH + "button", &clip, [&](Widget* tthis, ClickEvent& e) {
				if (e.getState() == MOUSE_CLICK) {
					m_moving = true;
					m_offsetWindowOrigin = e.getPosition(*tthis);
				} else if (e.getState() == MOUSE_RELEASE) {
					m_moving = false;
				}
			});
			auto wButtonWidth = this->getBox().w - TAILLEBLOCFENETRE / 2;
			button.setWidth((wButtonWidth > 0) ? wButtonWidth : this->getBox().w);
			button.setHeight(TAILLEBLOCFENETRE / 2);
			button.template addHandler<HoverEventListener> ([&](Widget*, HoverEvent& e) {
				if (m_moving && e.getState() != MOUSE_ENTER) {
					const auto& clickAbsPos = e.getMousePosition();
					const auto& newPos = clickAbsPos - m_offsetWindowOrigin;
					this->move(newPos);
				}
			});

			auto& buttonQuit = this->template addWidget<ButtonQuit>(Point<int>(this->getBox().w - TAILLEBLOCFENETRE / 2, 0), Button::MENU_DEFAULT_THEME_PATH + "close_button");
			buttonQuit.setPriority(std::numeric_limits<int>::max() - 1);

			button.setPriority(std::numeric_limits<int>::max());

		}

		bool m_moving;
		Point<int> m_offsetWindowOrigin;
	};
}
