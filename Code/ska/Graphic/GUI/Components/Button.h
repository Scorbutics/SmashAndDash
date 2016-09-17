#pragma once

#include <functional>
#include "../../Texture.h"
#include "Widget.h"
#include "ButtonState.h"
#include "MouseObserver.h"


namespace ska {

	class MouseObservable;
	class InputContextManager;

	class Button :
		public Widget {
	public:
		Button(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, ClickEventHandler const& callback);
		virtual void display() const override;
		virtual ~Button();
	protected:
		Button();
		Button(Widget& parent);
		
	private:
		void initHandlers();
		ska::Texture m_placeHolder;
		ska::Texture m_placeHolderHover;
		ska::Texture m_placeHolderPressed;
		ska::ButtonState::Enum m_state;
		ska::Texture* m_textureSelector;
		bool m_drawStyle;
	};
}
