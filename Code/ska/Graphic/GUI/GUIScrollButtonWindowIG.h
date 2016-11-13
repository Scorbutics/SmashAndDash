#pragma once
#include "DynamicWindowIG.h"
#include "Components/ClickEventListener.h"
#include "Components/HoverEventListener.h"

namespace ska {
	class GUIScrollButtonWindowIG : public DynamicWindowIG<> {
	public:
		GUIScrollButtonWindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle);
		virtual bool scrollTo(const ska::Point<int>& targetPos, unsigned int speed);
		bool scrollRewind();
		virtual ~GUIScrollButtonWindowIG();
	private:
		Point<int> m_realPos;

	};
}