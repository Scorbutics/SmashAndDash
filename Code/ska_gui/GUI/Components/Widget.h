#pragma once

#include "Point.h"
#include "Draw/DrawableFixedPriority.h"
#include "Utils/RectangleUtils.h"

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
		public DrawableFixedPriority {

	public:
		Widget();
		explicit Widget(Widget& parent);

		Widget(Widget& parent, Point<int>& position);

		bool isVisible() const override;

		virtual void setWidth(unsigned int w);

		virtual void setHeight(unsigned int h);

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

		Point<int> getAbsolutePosition() const;

		Point<int> getRelativePosition() const;

		virtual ~Widget() = default;

		void focus(bool f);

		bool isFocused() const;
	private:
		std::string m_name;
		Widget* m_parent;
		Rectangle m_box;
		bool m_visible;

		bool m_focused;
	};

}
