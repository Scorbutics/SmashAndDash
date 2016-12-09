#pragma once
#include <unordered_set>
#include "RadioButton.h"

namespace ska {
	class RadioButtonGroup {
	public:
		RadioButtonGroup();
		void addRadioButton(RadioButton& rb);
		void select(RadioButton& rb);
		void removeRadioButton(RadioButton& rb);
		~RadioButtonGroup() = default;

	private:
		std::unordered_set<RadioButton*> m_buttons;
	};
}