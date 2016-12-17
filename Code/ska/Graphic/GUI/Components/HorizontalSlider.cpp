#include <memory>
#include "Button.h"
#include "Image.h"
#include "HorizontalSlider.h"

ska::HorizontalSlider::HorizontalSlider(Widget& parent, const std::string& styleName, Point<int> relativePos, const unsigned int pixelWidth) : 
WidgetPanel<ValueChangedEventListener<float>, HoverEventListener, ClickEventListener>(parent, relativePos),
	m_sliding(false), 
	m_percents(0) {
		auto b = std::unique_ptr<Button>(new Button(*this, ska::Point<int>(0, 0), styleName, nullptr, [this](Widget* tthis, ClickEvent& e) {
			m_sliding = (e.getState() == MouseEventType::MOUSE_CLICK);
		}));

	ska::Rectangle clipImage{ 0 };
	clipImage.w = pixelWidth;
	clipImage.h = std::numeric_limits<int>::max();
	auto img = std::unique_ptr<Image>(new Image(*this, styleName + "_bar.png", ska::Point<int>(0, 0), false, &clipImage));
	img->setWidth(pixelWidth);
	setWidth(pixelWidth);
	setHeight(img->getBox().h);

	addHandler<HoverEventListener>([this](Widget* tthis, HoverEvent& e) {
		if(e.getState() == MouseEventType::MOUSE_OVER && m_sliding) {
			const auto lastValue = m_percents;
			m_percents = ((e.getMousePosition().x - getAbsolutePosition().x) / (float)getBox().w);

			if (m_percents > 100.0) {
				m_percents = 100.0;
			} else if (m_percents < 0) {
				m_percents = 0;
			}

			auto button = getWidget(1);
			const auto& buttonPos = button->getRelativePosition();
			auto newPos = ska::Point<int>(m_percents * getBox().w - button->getBox().w / 2, buttonPos.y);
			button->move(newPos);
			
			ValueChangedEvent<float> vce(lastValue, m_percents);
			directNotify(vce);
		}
	});

	addWidget(img);
	addWidget(b);
}

float ska::HorizontalSlider::getValue() const {
	return m_percents;
}
