#pragma once
#include "Label.h"
#include "WidgetPanel.h"
#include "TimeEvent.h"


namespace ska {
	class ScrollingMultiLineLabel : public WidgetPanel<TimeEventListener> {
	public:
		void buildTextLines(const std::string& cs);
		ScrollingMultiLineLabel(Widget& parent, const std::string& text, int fontSize, float scrollSpeed, Point<int> relativePos, unsigned int lineWidth);
		ScrollingMultiLineLabel& operator=(const ScrollingMultiLineLabel&) = delete;
		void refresh();
		virtual ~ScrollingMultiLineLabel() = default;
	private:
		Rectangle m_displayingArea;
		unsigned int m_currentLine;
		std::vector<std::string> m_textLines;
		const int m_fontSize;
		const float m_scrollSpeed;
	};
}
