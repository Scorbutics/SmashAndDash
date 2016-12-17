#include "RadioButton.h"
#include "RadioButtonGroup.h"

ska::RadioButton::RadioButton(Widget& parent, std::shared_ptr<RadioButtonGroup>& group, ska::Point<int> relativePos, const std::string& placeHolderStyleName) :
	CheckBox(parent, relativePos, placeHolderStyleName, nullptr),
	m_group(group) {
	m_indexInGroup = m_group->addRadioButton(*this);

	addHandler<ClickEventListener>([&](Widget* tthis, ClickEvent& e) {
		if (e.getState() == MouseEventType::MOUSE_RELEASE) {
			if (getValue()) {
				m_group->select(*this);
			} else {
				forceValue(true);
			}
		}
	});
}

ska::RadioButton::~RadioButton() {
	m_group->removeRadioButton(*this);
}

