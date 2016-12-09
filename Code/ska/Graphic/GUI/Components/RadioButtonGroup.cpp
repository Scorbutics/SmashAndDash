#include "RadioButtonGroup.h"

ska::RadioButtonGroup::RadioButtonGroup() {	
}

void ska::RadioButtonGroup::addRadioButton(RadioButton& rb) {
	m_buttons.emplace(&rb);
}

void ska::RadioButtonGroup::select(RadioButton& rb) {
	for (auto& b : m_buttons) {
		if (b != &rb) {
			b->forceValue(false);
		}
	}
}

void ska::RadioButtonGroup::removeRadioButton(RadioButton& rb) {
	m_buttons.erase(&rb);
}
