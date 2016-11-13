#pragma once

#include "../../Point.h"
#include "../../Draw/DrawableFixedPriority.h"
#include "../../../Utils/RectangleUtils.h"
#include "../../../Utils/TupleUtils.h"

namespace ska {
	class IWidgetEvent;

	namespace ButtonState {
		enum Enum {
			NONE,
			PRESSED,
			HOVER,
			ENTER,
			DISABLED
		};
	}
	
	class Widget : 
		public DrawableFixedPriority{
		
	public:
		Widget();
		Widget(Widget& parent);

		Widget(Widget& parent, Point<int>& position);

		bool isVisible() const override;

		void setWidth(unsigned int w);

		void setHeight(unsigned int h);

		void show(bool sh);

		virtual void move(const Point<int>& pos);

		Widget* getParent() const;

		bool isAParent(const Widget& w) const;

		/* DEBUG ONLY */
		void setName(const std::string& s);

		const std::string& getName() const;
		/* END DEBUG ONLY */

		const Rectangle& getBox() const;

		virtual bool notify(IWidgetEvent& e);
		virtual bool directNotify(IWidgetEvent& e) {
			return notify(e);
		}

		const Point<int> getAbsolutePosition() const;

		const Point<int> getRelativePosition() const;

		virtual ~Widget() = default;

	private:
		std::string m_name;
		Widget* m_parent;
		Rectangle m_box;
		bool m_visible;
	};

}
