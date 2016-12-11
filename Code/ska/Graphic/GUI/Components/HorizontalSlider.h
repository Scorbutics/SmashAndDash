#pragma once

#include "HoverEventListener.h"
#include "ClickEventListener.h"
#include "WidgetPanel.h"

namespace ska {
	class HorizontalSlider : public WidgetPanel<HoverEventListener, ClickEventListener> {
	public:
		HorizontalSlider(Widget& parent, const std::string& styleName, Point<int> relativePos, const unsigned int pixelWidth);
		
		float getValue() const;

		virtual ~HorizontalSlider() = default;

	private:
		float m_percents;
		bool m_sliding;
	};
}
