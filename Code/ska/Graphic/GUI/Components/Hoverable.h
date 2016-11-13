#pragma once
#include <iostream>

#include "../../../Utils/SkaConstants.h"

#include "HoverEventListener.h"

#include "HandledWidget.h"

namespace ska {

	class MouseObservable;
	class InputContextManager;

	template <class W>
	class HoverStateController {
	public:
		HoverStateController() : m_state(ska::ButtonState::NONE) {
			W& asW = (W&)*this;
			asW.addHeadHandler<HoverEventListener>(std::bind(&HoverStateController<W>::hoverEvent, (W*)this, std::placeholders::_1, std::placeholders::_2));
		}
	
		virtual ~HoverStateController() = default;

		void hoverEvent(Widget* tthis, HoverEvent& e) {
			W& asW = (W&)*this;
			bool handled = true;
			bool target = false;
			switch (e.getState()) {
			case ska::MouseEventType::MOUSE_ENTER:
				if (m_state != ButtonState::HOVER && m_state != ButtonState::ENTER && m_state != ButtonState::PRESSED) {
					m_state = ButtonState::ENTER;
					//std::clog << "Enter " << asW.getName() << std::endl;
					switchTextureAndMemorize();
					target = true;
				}
				else {
					handled = false;
				}
				break;

			case ska::MouseEventType::MOUSE_OUT:
				if (m_state == ButtonState::HOVER || m_state == ButtonState::ENTER || m_state == ButtonState::PRESSED) {
					m_state = ButtonState::NONE;
					//std::clog << "Out " << asW.getName() << std::endl;
					switchTextureAndMemorize();
					target = true;
				}
				else {
					handled = false;
				}
				break;

			case ska::MouseEventType::MOUSE_OVER:
				m_state = ButtonState::HOVER;
				target = true;
				break;

			default:
				break;

			}

			if (!handled) {
				/* Refuses the current event (not handled) */
				e.stopPropagation(ska::StopType::STOP_CALLBACK);
			}

			if (target && e.getTarget() == nullptr) {
				e.setTarget(&asW);
			}
		}

		virtual void switchTextureAndMemorize() {}
		virtual void resetTexture() {}

	protected:
		ska::ButtonState::Enum m_state;
	};


	template <class ...HL>
	class Hoverable :
		public HandledWidget<HoverEventListener, HL...>,
		public HoverStateController<Hoverable<HL...>> {
		friend class HoverStateController<Hoverable<HL...>>;
	public:
		Hoverable(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const ska::Rectangle* clip) :
			HandledWidget<HoverEventListener, HL...>(parent) {
			move(getRelativePosition() + relativePos);
		}
	
		Hoverable(Widget& parent) :
			HandledWidget<HoverEventListener, HL...>(parent) {
			}

		virtual void display() const override {
		}

		virtual ~Hoverable() = default;

	protected:
		Hoverable() {

		}

	};
}
