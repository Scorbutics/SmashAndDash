// #pragma once
// 
// #include <iostream>
// #include <string>
// #include "../../Point.h"
// 
// class Widget;
// 
// namespace ska {
// 	class WindowArea {
// 	public:
// 		WindowArea(Widget& parent);
// 		virtual ~WindowArea();
// 
// 		bool canBeClicked();
// 		bool isVisible();
// 		void hide(bool x);
// 		ska::Point<int>& getRelativePos();
// 		void setRelativePos(ska::Point<int> pos);
// 		DialogMenu& getParent();
// 		ska::Point<int> getAbsolutePos();
// 
// 		virtual void display() const = 0;
// 
// 	protected:
// 		Widget& m_parent;
// 		ska::Point<int> m_relativePos;
// 		bool m_canBeClicked, m_active;
// 		int m_type;
// 
// 	};
// }
