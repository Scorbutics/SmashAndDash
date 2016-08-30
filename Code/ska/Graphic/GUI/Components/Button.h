#pragma once

#include <functional>
#include "../../Texture.h"
#include "Widget.h"
#include "ButtonState.h"
#include "HasClickHandler.h"
#include "HasHoverHandler.h"
#include "../../../Utils/Observer.h"



namespace ska {
	namespace ButtonState {
		enum Enum {
			NONE,
			PRESSED,
			HOVER,
			DISABLED
		};
	}

	class GUI;
	class InputContextManager;
	class ClickEvent;
	class HoverEvent;
	using ClickEventHandler = std::function<void(const ska::ClickEvent&)>;

	using ClickObserver = Observer<ClickEvent>;
	using HoverObserver = Observer<HoverEvent>;

	class Button : 
		public Widget, 
		public HasClickHandler, 
		public HasHoverHandler,
		public ClickObserver,
		public HoverObserver {

	public:
		Button(GUI& guiObservable, Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, ClickEventHandler const& callback);
		virtual void display() const override;
		virtual ~Button();

		void click(const ska::ClickEvent& e) override;

		void mouseHover(const ska::HoverEvent& e) override;

	private:
		ska::ButtonState::Enum m_state;
		ska::Texture m_placeHolder;
		ska::Texture m_placeHolderHover;
		ska::Texture m_placeHolderPressed;

		ska::Texture* m_textureSelector;
		bool m_drawStyle;
		ClickEventHandler m_callback;
		GUI& m_guiObservable;
	};
}
