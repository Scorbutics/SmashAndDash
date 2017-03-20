#pragma once
#include "../../Rectangle.h"
#include "../../Point.h"
#include "TimeScrollableWindowIG.h"

namespace ska {

	class GUIScrollButtonWindowIG : public TimeScrollableWindowIG<> {
	public:
		GUIScrollButtonWindowIG(Widget& parent, const Rectangle& box, const std::string& styleName);
		virtual bool scrollTo(const Point<int>& targetPos, unsigned int speed) override;
		bool scrollRewind();
		virtual ~GUIScrollButtonWindowIG();
	private:
		Point<int> m_realPos;

	};
}
