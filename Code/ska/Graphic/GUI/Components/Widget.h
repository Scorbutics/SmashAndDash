#pragma once

#include "../../Point.h"
#include "../../Draw/DrawableFixedPriority.h"

namespace ska {
	class Widget : public DrawableFixedPriority {
	public:
		Widget();
		Widget(Widget& parent);
		Widget(Widget& parent, Point<int>& position);
	
		/* override DrawableFixedPriority */
		bool isVisible() const override;

		void setWidth(unsigned int w);
		void setHeight(unsigned int h);

		void show(bool sh);
		void move(const ska::Point<int>& pos);

		virtual ~Widget() = default;

		const Widget* getParent() const;

		const ska::Rectangle& getBox() const;
		const Point<int> getAbsolutePosition() const;
		const Point<int> getRelativePosition() const;

	private:
		Widget* m_parent;
		ska::Rectangle m_box;
		bool m_visible;
	};
}
