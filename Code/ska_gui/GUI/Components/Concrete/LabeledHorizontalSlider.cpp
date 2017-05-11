
#include "LabeledHorizontalSlider.h"
#include "Utils/StringUtils.h"

ska::LabeledHorizontalSlider::LabeledHorizontalSlider(Widget& parent, const std::string& styleName, Point<int> relativePos, const unsigned int pixelWidth):
	HorizontalSlider(parent, styleName, relativePos, pixelWidth),
	m_oldIntValue(0),
	m_label(addWidget<Label>("0%", 14, Point<int>(pixelWidth + 20, 0))) {

}

void ska::LabeledHorizontalSlider::forceValue(float v) {
	HorizontalSlider::forceValue(v);
	auto newIntValue = static_cast<int>(v * 100);
	if (newIntValue != m_oldIntValue) {
		m_oldIntValue = newIntValue;
		m_label.modifyText(StringUtils::intToStr(newIntValue) + "%");
	}
}
