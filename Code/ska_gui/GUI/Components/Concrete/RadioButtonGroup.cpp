#include "RadioButtonGroup.h"

ska::RadioButtonGroup::RadioButtonGroup() : 
	m_uniqueIndexCursor(0) {
}

size_t ska::RadioButtonGroup::addRadioButton(RadioButton& rb) {
	m_buttons.emplace(&rb);
	return m_uniqueIndexCursor++;
}

void ska::RadioButtonGroup::select(RadioButton& rb) {
	for (auto& b : m_buttons) {
		if (b != &rb) {
			b->forceValue(false);
		}
	}
}

size_t ska::RadioButtonGroup::getSize() const {
	return m_buttons.size();
}

void ska::RadioButtonGroup::removeRadioButton(RadioButton& rb) {
	m_buttons.erase(&rb);
}

