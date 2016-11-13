#pragma once

#include <functional>
#include "../../Texture.h"
#include "Widget.h"
#include "MouseObserver.h"
#include "../../../Utils/SkaConstants.h"
#include "ClickEventListener.h"
#include "Hoverable.h"

namespace ska {

	class MouseObservable;
	class InputContextManager;

	class Button :
		public Hoverable<ClickEventListener> {
	public:
		Button(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const ska::Rectangle* clip, ClickEventHandler const& callback);
		virtual void display() const override;
		virtual ~Button();

		static const std::string& MENU_DEFAULT_THEME_PATH;
	
	protected:
		Button();
		Button(Widget& parent);
		virtual void switchTextureAndMemorize() override;
		virtual void resetTexture() override;

	private:
		void initHandlers();

		ska::Texture m_placeHolder;
		ska::Texture m_placeHolderHover;
		ska::Texture m_placeHolderPressed;
		ska::Texture const* m_textureSelector;
		ska::Texture const* m_lastTextureSelector;
		bool m_drawStyle;
		ska::Rectangle m_clip;
	};
}
