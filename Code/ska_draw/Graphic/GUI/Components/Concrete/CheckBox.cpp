
#include "CheckBox.h"
#include "../../Events/ValueChangedEvent.h"

ska::CheckBox::CheckBox(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const Rectangle* clip) :
	Button(parent, relativePos, placeHolderStyleName, clip, [&](Widget*, ClickEvent& e) {
		if (e.getState() == MOUSE_RELEASE) {
			m_value = !m_value;
			
			ValueChangedEvent<bool> vce(!m_value, m_value);
			directNotify(vce);
		}
	}),
	m_value(false),
	m_check(placeHolderStyleName + "_check.png") {
}

void ska::CheckBox::display() const {
	Button::display();
	if(m_value) {
		const auto& absPos = getAbsolutePosition();
		m_check.render(absPos.x + (getBox().w - m_check.getWidth()) / 2, absPos.y + (getBox().h - m_check.getHeight()) / 2);
	}
}

bool ska::CheckBox::getValue() const {
	return m_value;
}

void ska::CheckBox::forceValue(bool b) {
	m_value = b;
}

