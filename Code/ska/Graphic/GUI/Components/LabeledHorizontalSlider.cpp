#include "../../../Utils/StringUtils.h"
#include "LabeledHorizontalSlider.h"

ska::LabeledHorizontalSlider::LabeledHorizontalSlider(Widget& parent, const std::string& styleName, Point<int> relativePos, const unsigned int pixelWidth):
	HorizontalSlider(parent, styleName, relativePos, pixelWidth),
	m_oldIntValue(0) {

	auto ref = std::unique_ptr<Label>(new Label(*this, "0%", 14, Point<int>(pixelWidth + 20, 0)));
	m_label = addWidget(ref);

}

void ska::LabeledHorizontalSlider::forceValue(float v) {
	HorizontalSlider::forceValue(v);
	auto newIntValue = (int)(v * 100);
	if (newIntValue != m_oldIntValue) {
		m_oldIntValue = newIntValue;
		m_label->modifyText(StringUtils::intToStr(newIntValue) + "%");
	}
}
