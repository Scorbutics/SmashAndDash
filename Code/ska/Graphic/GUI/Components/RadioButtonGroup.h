#pragma once
#include <unordered_set>
#include "RadioButton.h"

namespace ska {
	class RadioButtonGroup {
	public:
		RadioButtonGroup();
		std::size_t addRadioButton(RadioButton& rb);
		void select(RadioButton& rb);
		std::size_t getSize() const;
		void removeRadioButton(RadioButton& rb);
		~RadioButtonGroup() = default;

	private:
		std::unordered_set<RadioButton*> m_buttons;
		std::size_t m_uniqueIndexCursor;
	};
}