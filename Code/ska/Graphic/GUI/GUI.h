#pragma once
#include <vector>
#include <memory>
#include "../../Graphic/Draw/DrawableFixedPriority.h"
#include "Utils/MouseObservable.h"
#include "Windows/DynamicWindowIG.h"
#include "Components/Concrete/MouseCursor.h"
#include "Utils/TimeObservable.h"
#include "Windows/TimeScrollableWindowIG.h"


namespace ska {
	class Window;
	class ClickEvent;
	class HoverEvent;
	class InputContextManager;

	class GUI : 
		public DrawableFixedPriority, 
		public MouseObservable, 
		public KeyObservable,
		public TimeObservable {

	public:
		GUI(const Window& w, InputContextManager& playerICM);

		GUI& operator=(const GUI&) = delete;

		void refresh();
		bool isVisible() const override;
		void initButtons(const Window& w);
		void hide(bool x);

		void display() const override;
		

	private:
		void refreshMouse();
		void refreshKeyboard();
		void windowSorter(Widget* tthis, ClickEvent& e);

		TimeScrollableWindowIG<>* m_wAction;
		std::vector<std::unique_ptr<Widget>> m_topWindowWidgets;
		bool m_hide;
		const Window& m_window;

		InputContextManager& m_playerICM;

		Widget* m_hovered;
		Widget* m_clicked;
		Widget* m_lastFocused;
		MouseCursor m_mouseCursor;

	protected:
		template <class Win, class ...HL>
		Win* addWindow(std::unique_ptr<Win>&& w, const std::string& name) {
			auto result = m_wMaster.addWidget(std::move(w));
			auto t = reinterpret_cast<DynamicWindowIG<HL...>*>(result);
			m_windowAnnuary[name] = t;
			t->template addHeadHandler<ClickEventListener>([&](Widget* tthis, ClickEvent& e) {
				windowSorter(tthis, e);
			});
			return result;
		}

		void pushWindowToFront(Widget* w);
		Widget* addTopWidget(std::unique_ptr<Widget>& w);


		Widget* getWindow(const std::string& name) {
			return m_windowAnnuary[name];
		}

		TimeScrollableWindowIG<KeyEventListener> m_wMaster;
		std::unordered_map<std::string, Widget*> m_windowAnnuary;
	};
}
