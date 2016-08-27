#pragma once

#include <functional>
#include "../../Texture.h"
#include "Widget.h"
#include "ButtonState.h"
#include "HasClickHandler.h"
#include "HasHoverHandler.h"

namespace ska {
	class InputContextManager;
	class ClickEvent;
	using ClickEventHandler = std::function<void(const ska::ClickEvent&)>;
	

	class Button : 
		public Widget, 
		public HasClickHandler, 
		public HasHoverHandler {

	public:
		Button(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, ClickEventHandler const& callback);
		virtual void display() const override;
		virtual ~Button();

		void click(const ska::ClickEvent& e) override;
		void release(const ska::ClickEvent& e) override;

		void mouseHover() override;
		void mouseOut() override;

	private:
		ButtonState::Enum m_state;
		ska::Texture m_placeHolder;
		ska::Texture m_placeHolderHover;
		ska::Texture m_placeHolderPressed;

		ska::Texture* m_textureSelector;

		ClickEventHandler m_callback;
	};
}
