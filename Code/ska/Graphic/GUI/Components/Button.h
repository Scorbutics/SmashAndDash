#pragma once

#include <functional>
#include "../../Texture.h"
#include "Widget.h"
#include "ButtonState.h"
#include "MouseObserver.h"
#include "../../../Utils/SkaConstants.h"

namespace ska {

	class MouseObservable;
	class InputContextManager;

	class Button :
		public Widget {
	public:
		Button(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const ska::Rectangle* clip, ClickEventHandler const& callback);
		virtual void display() const override;
		virtual ~Button();

		static const std::string& MENU_DEFAULT_THEME_PATH;
	
	protected:
		Button();
		Button(Widget& parent);
		
	private:
		void switchTextureAndMemorize(const ska::Texture& t);
		void initHandlers();

		ska::Texture m_placeHolder;
		ska::Texture m_placeHolderHover;
		ska::Texture m_placeHolderPressed;
		ska::ButtonState::Enum m_state;
		ska::Texture const* m_textureSelector;
		ska::Texture const* m_lastTextureSelector;
		bool m_drawStyle;
		ska::Rectangle m_clip;
	};
}
