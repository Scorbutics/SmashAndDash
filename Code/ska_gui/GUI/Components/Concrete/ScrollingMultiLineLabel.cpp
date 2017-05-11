#include "ScrollingMultiLineLabel.h"
#include "Label.h"
#include "Utils/StringUtils.h"


ska::ScrollingMultiLineLabel::ScrollingMultiLineLabel(Widget& parent, const std::string& text, int fontSize, float scrollSpeed, Point<int> relativePos) :
	WidgetPanel(parent, relativePos), m_displayingArea({ 0, 0, 0, 0 }), m_currentLine(0),
	m_fontSize(fontSize),
	m_scrollSpeed(scrollSpeed < 1.F ? 1.F : scrollSpeed), m_stopped(false) {

	buildTextLines(text);

	Widget::setHeight(static_cast<unsigned int>(m_textLines.size() * fontSize));

	auto& firstLineLabel = addWidget<Label>(m_textLines.empty() ? " " : m_textLines[m_currentLine], m_fontSize, Point<int>());
	addHandler<TimeEventListener>([&](Widget*, TimeEvent&) {
		refresh();
	});
	m_displayingArea.h = firstLineLabel.getBox().h;
}

void ska::ScrollingMultiLineLabel::refresh() {
	if(m_stopped) {
		return;
	}
	auto currentLabelLine = static_cast<Label*>(getWidget(m_currentLine));

	m_displayingArea.w += static_cast<int>(m_scrollSpeed);
	const auto percentsWidth = static_cast<float>(m_displayingArea.w) * 100 / currentLabelLine->getBox().w;
	if(percentsWidth >= 100.F) {
		if (m_currentLine + 1 >= m_textLines.size()) {
			m_stopped = true;
			return;
		}
		m_displayingArea.w = 0;
		currentLabelLine->setClip(nullptr);

		m_currentLine++;
		addWidget<Label>(m_textLines.empty() ? " " : m_textLines[m_currentLine], m_fontSize, Point<int>(0, m_fontSize * (m_currentLine)));
		currentLabelLine = static_cast<Label*>(getWidget(m_currentLine));
	}
	currentLabelLine->setClip(&m_displayingArea);
}

void ska::ScrollingMultiLineLabel::buildTextLines(const std::string& cs) {
	m_textLines = StringUtils::split(cs, '\n');
}
