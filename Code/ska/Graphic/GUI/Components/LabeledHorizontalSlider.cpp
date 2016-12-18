#include "../../../Utils/StringUtils.h"
#include "LabeledHorizontalSlider.h"

ska::LabeledHorizontalSlider::LabeledHorizontalSlider(Widget& parent, const std::string& styleName, Point<int> relativePos, const unsigned int pixelWidth):
	HorizontalSlider(parent, styleName, relativePos, pixelWidth) {

	m_label = addWidget(std::unique_ptr<Label>(new Label(*this, "0%", 14, ska::Point<int>(pixelWidth + 20, 0))));
	addHandler<ValueChangedEventListener<float>>([this](Widget* tthis, ValueChangedEvent<float>& e) {
		m_label->modifyText(ska::StringUtils::intToStr((int)(e.getValue()*100)) + "%");
	});
	
}
