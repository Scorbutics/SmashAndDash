#pragma once

#include "../Hoverable.h"
#include "../../Events/ClickEventListener.h"
#include "Graphic/Texture.h"
#include "../../Events/ValueChangedEvent.h"


namespace ska {

	class Button :
		public Hoverable<ValueChangedEventListener<bool>, ClickEventListener> {
	public:
		Button(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const Rectangle* clip, ClickEventHandler const& callback);
		virtual void display() const override;
		virtual ~Button();

		static const std::string& MENU_DEFAULT_THEME_PATH;

		void forceState(ButtonState::Enum e);

	protected:
		Button();
		explicit Button(Widget& parent);
		virtual void switchTextureAndMemorize() override;
		virtual void resetTexture() override;

	private:
		void initHandlers();

		Texture m_placeHolder;
		Texture m_placeHolderHover;
		Texture m_placeHolderPressed;
		Texture const* m_textureSelector;
		Texture const* m_lastTextureSelector;
		bool m_drawStyle;
		Rectangle m_clip;

	};
}
