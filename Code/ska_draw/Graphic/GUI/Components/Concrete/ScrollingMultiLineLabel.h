#pragma once
#include "../WidgetPanel.h"
#include "../../Events/TimeEventListener.h"


namespace ska {
	class ScrollingMultiLineLabel : public WidgetPanel<TimeEventListener> {
	public:
		void buildTextLines(const std::string& cs);
		ScrollingMultiLineLabel(Widget& parent, const std::string& text, int fontSize, float scrollSpeed, Point<int> relativePos);
		ScrollingMultiLineLabel& operator=(const ScrollingMultiLineLabel&) = delete;
		virtual ~ScrollingMultiLineLabel() = default;
	private:
		void refresh();
		Rectangle m_displayingArea;
		unsigned int m_currentLine;
		std::vector<std::string> m_textLines;
		const int m_fontSize;
		const float m_scrollSpeed;
		bool m_stopped;
	};
}
