#pragma once

#include <functional>
#include "../../Texture.h"
#include "Widget.h"
#include "ButtonState.h"
#include "MouseObserver.h"


namespace ska {
	namespace ButtonState {
		enum Enum {
			NONE,
			PRESSED,
			HOVER,
			DISABLED
		};
	}

	class MouseObservable;
	class InputContextManager;
	using ClickEventHandler = std::function<void(const ska::ClickEvent&)>;

	class Button :
		public Widget, 
		public MouseObserver {
	public:
		Button(MouseObservable& guiObservable, Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, ClickEventHandler const& callback);
		virtual void display() const override;
		virtual ~Button();

		bool click(ska::ClickEvent& e) override;
		bool mouseHover(ska::HoverEvent& e) override;

	private:
		ska::ButtonState::Enum m_state;
		ska::Texture m_placeHolder;
		ska::Texture m_placeHolderHover;
		ska::Texture m_placeHolderPressed;

		ska::Texture* m_textureSelector;
		bool m_drawStyle;
		ClickEventHandler m_callback;
		MouseObservable& m_guiObservable;
	};
}
