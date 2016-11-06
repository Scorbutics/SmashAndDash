#pragma once

#include <deque>
#include "../../Point.h"
#include "../../Draw/DrawableFixedPriority.h"
#include "MouseObserver.h"
#include "KeyEvent.h"


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

	/* Handler triggered when a click occurs on the widget */
	using ClickEventHandler = std::function<void(ska::Widget*, ska::ClickEvent&)>;
	
	/* Handler triggered when hovering on the widget */
	using HoverEventHandler = std::function<void(ska::Widget*, ska::HoverEvent&)>;

	/* Handler triggered when pressing a key */
	using KeyEventHandler = std::function<void(ska::Widget*, ska::KeyEvent&)>;

	using KeyObserver = Observer<KeyEvent>;

	class Widget : 
		public DrawableFixedPriority,
		//TODO : déplacer MouseObserver et KeyboardObserver aux widgets qui nécessitent vraiment (classe de traits ?)
		public MouseObserver,
		public KeyObserver {
		
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
		
		//TODO classe de traits ? => typage au compile time donc ça se prête plutôt bien à ça
		virtual bool isAffectedBy(const ska::HoverEvent& e) const;
		virtual bool isAffectedBy(const ska::KeyEvent& e) const;

		virtual ~Widget() = default;

		Widget* getParent() const;

		bool click(ska::ClickEvent& e) override;
		bool mouseHover(ska::HoverEvent& e) override;
		
		bool keyEvent(ska::KeyEvent& e);

		const std::string& getName() const;
		bool isAParent(const ska::Widget& w) const;
		void setName(const std::string& s);

		void addClickHandler(const ClickEventHandler& h);
		void addHoverHandler(const HoverEventHandler& h);
		void addKeyHandler(const KeyEventHandler& h);

		const ska::Rectangle& getBox() const;
		const Point<int> getAbsolutePosition() const;
		const Point<int> getRelativePosition() const;

	protected:
		void addHeadClickHandler(const ClickEventHandler& h);
		void addHeadHoverHandler(const HoverEventHandler& h);
		void addHeadKeyHandler(const KeyEventHandler& h);

	private:
		std::string m_name;
		Widget* m_parent;
		ska::Rectangle m_box;
		bool m_visible;
		std::deque<ClickEventHandler> m_clickCallbacks;
		std::deque<HoverEventHandler> m_hoverCallbacks;
		std::deque<KeyEventHandler> m_keyCallbacks;
	};
}
