#pragma once

#include <deque>
#include "../../Point.h"
#include "../../Draw/DrawableFixedPriority.h"
#include "MouseObserver.h"



namespace ska {
	namespace ButtonState {
		enum Enum {
			NONE,
			PRESSED,
			HOVER,
			ENTER,
			DISABLED
		};
	}
	
	class Widget;

	using ClickEventHandler = std::function<bool(ska::Widget*, ska::ClickEvent&)>;
	using HoverEventHandler = std::function<bool(ska::Widget*, ska::HoverEvent&)>;

	class Widget : 
		public DrawableFixedPriority,
		public MouseObserver {
		
	public:
		Widget();
		Widget(Widget& parent);
		Widget(Widget& parent, Point<int>& position);
	
		/* override DrawableFixedPriority */
		bool isVisible() const override;

		void setWidth(unsigned int w);
		void setHeight(unsigned int h);

		void show(bool sh);
		virtual void move(const ska::Point<int>& pos);

		virtual ~Widget() = default;

		Widget* getParent() const;

		bool click(ska::ClickEvent& e) override;
		bool mouseHover(ska::HoverEvent& e) override;
		
		const std::string& getName() const;
		bool isAParent(const ska::Widget& w) const;
		void setName(const std::string& s);

		void addClickHandler(const ClickEventHandler& h);
		void addHoverHandler(const HoverEventHandler& h);

		const ska::Rectangle& getBox() const;
		const Point<int> getAbsolutePosition() const;
		const Point<int> getRelativePosition() const;

	protected:
		void addHeadClickHandler(const ClickEventHandler& h);
		void addHeadHoverHandler(const HoverEventHandler& h);

	private:
		std::string m_name;
		Widget* m_parent;
		ska::Rectangle m_box;
		bool m_visible;
		std::deque<ClickEventHandler> m_clickCallbacks;
		std::deque<HoverEventHandler> m_hoverCallbacks;
	};
}
