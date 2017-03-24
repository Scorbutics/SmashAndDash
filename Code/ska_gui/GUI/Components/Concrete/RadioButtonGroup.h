#pragma once
#include <unordered_set>
#include "RadioButton.h"

namespace ska {
	class RadioButtonGroup {
	public:
		RadioButtonGroup();
		size_t addRadioButton(RadioButton& rb);
		void select(RadioButton& rb);
		size_t getSize() const;
		void removeRadioButton(RadioButton& rb);
		~RadioButtonGroup() = default;

	private:
		std::unordered_set<RadioButton*> m_buttons;
		size_t m_uniqueIndexCursor;
	};
}
