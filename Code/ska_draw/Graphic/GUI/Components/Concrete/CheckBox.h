#pragma once
#include "Button.h"
namespace ska {
	class CheckBox : public Button {
	public:
		CheckBox(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const Rectangle* clip);
		virtual ~CheckBox() = default;
		virtual void display() const override;
		bool getValue() const;
		void forceValue(bool b);

	private:
		bool m_value;
		Texture m_check;
	};
}
