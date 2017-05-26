#include <memory>
#include "Button.h"
#include "Image.h"
#include "HorizontalSlider.h"
#include "../../Events/ValueChangedEvent.h"

ska::HorizontalSlider::HorizontalSlider(Widget& parent, const std::string& styleName, Point<int> relativePos, const unsigned int pixelWidth) :
WidgetPanel<ValueChangedEventListener<float>, HoverEventListener, ClickEventListener>(parent, relativePos),
	m_percents(0),
	m_sliding(false) {

	Rectangle clipImage{ 0, 0, 0, 0 };
	clipImage.w = pixelWidth;
	clipImage.h = std::numeric_limits<int>::max();

	auto& img = addWidget<Image>(styleName + "_bar.png", Point<int>(0, 0), false, &clipImage);
	img.setWidth(pixelWidth);

	addWidget<Button>(Point<int>(0, 0), styleName, nullptr, [this](Widget*, ClickEvent& e) {
		m_sliding = e.getState() == MOUSE_CLICK;
		if (m_sliding) {
			m_lastPercents = m_percents;
		} else {
			ValueChangedEvent<float> vce(m_lastPercents, m_percents);
			directNotify(vce);
		}
	});

	Widget::setWidth(pixelWidth);
	Widget::setHeight(img.getBox().h);

	addHandler<HoverEventListener>([this](Widget*, HoverEvent& e) {
		if(e.getState() == MOUSE_OVER && m_sliding) {
			forceValue(((e.getMousePosition().x - getAbsolutePosition().x) / (float)getBox().w));
		}
		if(e.getState() == MOUSE_OUT) {
			ValueChangedEvent<float> vce(m_lastPercents, m_percents);
			directNotify(vce);
		}
	});
}

float ska::HorizontalSlider::getValue() const {
	return m_percents;
}

void ska::HorizontalSlider::forceValue(float v) {
	m_percents = v;

	if (m_percents > 1.0) {
		m_percents = 1.0;
	} else if (m_percents < 0) {
		m_percents = 0;
	}

	auto button = getWidget(1);
	const auto& buttonPos = button->getRelativePosition();
	auto newPos = Point<int>(static_cast<int>(m_percents * getBox().w - button->getBox().w / 2), buttonPos.y);
	button->move(newPos);
}
