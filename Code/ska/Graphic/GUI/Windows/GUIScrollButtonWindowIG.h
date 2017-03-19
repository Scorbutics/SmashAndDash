#pragma once
#include "DynamicWindowIG.h"
#include "../../Rectangle.h"
#include "../../Point.h"

namespace ska {

	class GUIScrollButtonWindowIG : public DynamicWindowIG<> {
	public:
		GUIScrollButtonWindowIG(Widget& parent, const Rectangle& box, const std::string& styleName);
		virtual bool scrollTo(const Point<int>& targetPos, unsigned int speed) override;
		bool scrollRewind();
		virtual ~GUIScrollButtonWindowIG();
	private:
		Point<int> m_realPos;

	};
}
