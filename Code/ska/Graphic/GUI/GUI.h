#pragma once
#include <vector>
#include <memory>
#include "../../Graphic/Draw/DrawableFixedPriority.h"
#include "../../Graphic/Point.h"
#include "./Components/MouseObservable.h"
#include "WindowIG.h"

namespace ska {
	class Window;
	class ClickEvent;
	class HoverEvent;
	class InputContextManager;
	class TimeScrollableWindowIG;

	class GUI : public ska::DrawableFixedPriority, public MouseObservable {

	public:
		GUI(const ska::Window& w, const ska::InputContextManager& playerICM);

		bool existDialog(const unsigned int index) const;
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

	private:
		ska::WindowIG m_wAction;
		std::vector <ska::TimeScrollableWindowIG*> m_dynamicWindows;
		std::vector<std::unique_ptr<ska::WindowIG>> m_extraWindows;
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
	};
}