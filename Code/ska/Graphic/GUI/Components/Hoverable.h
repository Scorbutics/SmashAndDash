#pragma once

#include "HandledWidget.h"
#include "../Events/HoverEvent.h"


namespace ska {

	template <class W>
	class HoverStateController {
	public:
		HoverStateController() : m_state(ButtonState::NONE) {
			W& asW = static_cast<W&>(*this);
			asW.template addHeadHandler<HoverEventListener>
			(std::bind(&HoverStateController<W>::hoverEvent, static_cast<W*>(this), std::placeholders::_1, std::placeholders::_2));
		}

		virtual ~HoverStateController() = default;

		void hoverEvent(Widget*, HoverEvent& e) {
			W& asW = static_cast<W&>(*this);
			auto handled = true;
			auto target = false;
			switch (e.getState()) {
			case MOUSE_ENTER:
				if (m_state != ButtonState::HOVER && m_state != ButtonState::ENTER && m_state != ButtonState::PRESSED) {
					m_state = ButtonState::ENTER;
					//std::clog << "Enter " << asW.getName() << std::endl;
					switchTextureAndMemorize();
					target = true;
				} else {
					handled = false;
				}
				break;

			case MOUSE_OUT:
				if (m_state == ButtonState::HOVER || m_state == ButtonState::ENTER || m_state == ButtonState::PRESSED) {
					m_state = ButtonState::NONE;
					//std::clog << "Out " << asW.getName() << std::endl;
					switchTextureAndMemorize();
					target = true;
				} else {
					handled = false;
				}
				break;

			case MOUSE_OVER:
				m_state = ButtonState::HOVER;
				target = true;
				break;

			default:
				break;

			}

			if (!handled) {
				/* Refuses the current event (not handled) */
				e.stopPropagation(STOP_CALLBACK);
			}

			if (target && e.getTarget() == nullptr) {
				e.setTarget(&asW);
			}
		}

		virtual void switchTextureAndMemorize() {}
		virtual void resetTexture() {}

	protected:
		ButtonState::Enum m_state;
	};


	template <class ...HL>
	class Hoverable :
		public HandledWidget<HoverEventListener, HL...>,
		public HoverStateController<Hoverable<HL...>> {
		friend class HoverStateController<Hoverable<HL...>>;
	public:
		Hoverable(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const Rectangle* clip) :
			HandledWidget<HoverEventListener, HL...>(parent) {
			this->move(this->getRelativePosition() + relativePos);
		}

		explicit Hoverable(Widget& parent) :
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
