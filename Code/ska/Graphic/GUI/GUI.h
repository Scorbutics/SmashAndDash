#pragma once
#include <vector>
#include <unordered_set>
#include <memory>
#include "../../Graphic/Draw/DrawableFixedPriority.h"
#include "../../Graphic/Point.h"
#include "./Components/MouseObservable.h"
#include "./Components/KeyObservable.h"
#include "DynamicWindowIG.h"
#include "./Components/KeyEventListener.h"

namespace ska {
	class Window;
	class ClickEvent;
	class HoverEvent;
	class InputContextManager;

	class GUI : 
		public ska::DrawableFixedPriority, 
		public MouseObservable, 
		public KeyObservable {

	public:
		GUI(const ska::Window& w, ska::InputContextManager& playerICM);

		void refresh();
		bool isVisible() const override;
		void initButtons(const ska::Window& w);
		void hide(bool x);

		void display() const override;
		
	protected:
		DynamicWindowIG<KeyEventListener> m_wMaster;
		std::unordered_map<std::string, Widget*> m_windowAnnuary;

	private:
		void refreshMouse();
		void refreshKeyboard();
		void windowSorter(Widget* tthis, ClickEvent& e);

		DynamicWindowIG<>* m_wAction;
		std::unordered_set<DynamicWindowIG<>*> m_dynamicWindows;
		std::vector<DynamicWindowIG<>*> m_extraWindows;
		bool m_hide;
		const ska::Window& m_window;

		ska::InputContextManager& m_playerICM;

		Widget* m_hovered;
		Widget* m_clicked;
		Widget* m_lastFocused;

	protected:
		template <class Win, class ...HL>
		void addWindow(std::unique_ptr<Win>& w, const std::string& name) {
			m_wMaster.addWidget(w);
			auto t = reinterpret_cast<DynamicWindowIG<HL...>*>(m_wMaster.backWidget());
			m_windowAnnuary[name] = t;
			t->addHeadHandler<ska::ClickEventListener>([&](Widget* tthis, ClickEvent& e) {
				windowSorter(tthis, e);
			});
		}
	};
}