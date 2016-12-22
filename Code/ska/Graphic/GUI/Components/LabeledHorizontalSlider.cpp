#include "../../../Utils/StringUtils.h"
#include "LabeledHorizontalSlider.h"

ska::LabeledHorizontalSlider::LabeledHorizontalSlider(Widget& parent, const std::string& styleName, Point<int> relativePos, const unsigned int pixelWidth):
	HorizontalSlider(parent, styleName, relativePos, pixelWidth),
	m_oldIntValue(0) {

	m_label = addWidget(std::unique_ptr<Label>(new Label(*this, "0%", 14, ska::Point<int>(pixelWidth + 20, 0))));

}

void ska::LabeledHorizontalSlider::forceValue(float v) {
	ska::HorizontalSlider::forceValue(v);
	auto newIntValue = (int)(v * 100);
	if (newIntValue != m_oldIntValue) {
		m_oldIntValue = newIntValue;
		m_label->modifyText(ska::StringUtils::intToStr(newIntValue) + "%");
	}
}
