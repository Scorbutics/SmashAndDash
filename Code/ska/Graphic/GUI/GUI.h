#pragma once
#include <vector>
#include <memory>
#include "../../Utils/Observer.h"
#include "../../Utils/Observable.h"
#include "../../Graphic/Draw/DrawableFixedPriority.h"
#include "../../Graphic/Point.h"
#include "../../Utils/Observable.h"
#include "WindowIG.h"

namespace ska {
	class Window;
	class ClickEvent;
	class HoverEvent;
	class InputContextManager;

	using ClickObservable = Observable<ClickEvent>;
	using HoverObservable = Observable<HoverEvent>;

	class GUI : public ska::DrawableFixedPriority, public ClickObservable, public HoverObservable {

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
	};
}