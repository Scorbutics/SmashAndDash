#pragma once
#include <memory>
#include "CheckBox.h"

namespace ska {
	class RadioButtonGroup;

	class RadioButton : public CheckBox {
	public:
		RadioButton(Widget& parent, std::shared_ptr<RadioButtonGroup>& group, ska::Point<int> relativePos, const std::string& placeHolderStyleName);
		virtual ~RadioButton();
	private:
		std::shared_ptr<RadioButtonGroup> m_group;
		std::size_t m_indexInGroup;
	};
}
