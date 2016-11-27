#pragma once
#include <vector>
#include <unordered_set>
#include <memory>
#include "../../Graphic/Draw/DrawableFixedPriority.h"
#include "../../Graphic/Point.h"
#include "./Components/MouseObservable.h"
#include "./Components/KeyObservable.h"
#include "DynamicWindowIG.h"

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
		GUI(const ska::Window& w, const ska::InputContextManager& playerICM);

		void refresh();
		int isPositionOnButton(const ska::Point<float>& pos) const;
		bool isMouseOnAWindow();
		bool isVisible() const override;
		void initButtons(const ska::Window& w);
		void hide(bool x);
		bool isMovingAWindow();

		void setClickMenu();
		void display() const override;

		void update();

	protected:
		DynamicWindowIG<> m_wMaster;
		std::unordered_map<std::string, DynamicWindowIG<>*> m_windowAnnuary;

	private:
		void refreshMouse();
		void refreshKeyboard();
		void windowSorter(Widget* tthis, ClickEvent& e);
		
		DynamicWindowIG<>* m_wAction;
		std::unordered_set<DynamicWindowIG<>*> m_dynamicWindows;
		std::vector<DynamicWindowIG<>*> m_extraWindows;
		std::vector<int> m_buttonScroll;
		ska::Point<int> m_lastMousePos;
		ska::Point<int> m_curObjectPos;
		int m_side;
		int m_lastMouseState;
		//int m_refreshCount;
		bool m_hide;
		bool m_isMovingWindow;
		const ska::Window& m_window;

		const ska::InputContextManager& m_playerICM;

		Widget* m_hovered;
		Widget* m_clicked;
		Widget* m_lastFocused;

	protected:
		template <class Win, class ...HL>
		void addWindow(std::unique_ptr<Win>& w, const std::string& name) {
			m_wMaster.addWidget(w);
			auto t = reinterpret_cast<DynamicWindowIG<HL...>*>(m_wMaster.backWidget());
			m_windowAnnuary[name] = t;
			t->addHeadHandler<ska::ClickEventListener>(std::bind(&GUI::windowSorter, this, std::placeholders::_1, std::placeholders::_2));
		}
	};
}