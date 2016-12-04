#pragma once
#include "Button.h"
namespace ska {
	class CheckBox : public Button {
	public:
		CheckBox(Widget& parent, ska::Point<int> relativePos, const std::string& placeHolderStyleName, const ska::Rectangle* clip);
		virtual ~CheckBox() = default;
		virtual void display() const override;
		bool getValue() const;

	private:
		bool m_value;
		Texture m_check;
	};
}