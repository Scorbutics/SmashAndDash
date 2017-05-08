#pragma once
#include <vector>
#include <memory>
#include "Draw/DrawableFixedPriority.h"
#include "Utils/MouseObservable.h"
#include "Windows/DynamicWindowIG.h"
#include "Components/Concrete/MouseCursor.h"
#include "Utils/TimeObservable.h"
#include "Windows/TimeScrollableWindowIG.h"
#include "Data/Events/GameEventDispatcher.h"

namespace ska {
	class Window;
	class ClickEvent;
	class HoverEvent;
	class InputContextManager;
    struct GUIEvent;

	class GUI :
		public DrawableFixedPriority,
		public MouseObservable,
		public KeyObservable,
		public TimeObservable,
		public Observer<GUIEvent> {

	public:
		GUI(GameEventDispatcher& ged, const Window& w, InputContextManager& playerICM);
        virtual ~GUI();
		GUI& operator=(const GUI&) = delete;

		void refresh();
		bool isVisible() const override;
		void hide(bool x);
		void display() const override;


	private:
		void refreshMouse();
		void refreshKeyboard();
		void windowSorter(Widget* tthis, ClickEvent& e);

		bool onGUIEvent(GUIEvent& ge);

		std::vector<std::unique_ptr<Widget>> m_topWindowWidgets;
		bool m_hide;
		const Window& m_window;

		InputContextManager& m_playerICM;

		Widget* m_hovered;
		Widget* m_clicked;
		Widget* m_lastFocused;
		MouseCursor m_mouseCursor;

		std::vector<std::string> m_windowsToDelete;

		TimeScrollableWindowIG<>* m_wAction;
		std::unordered_map<std::string, Widget*> m_windowAnnuary;

		GameEventDispatcher& m_ged;
        TimeScrollableWindowIG<KeyEventListener> m_wMaster;

	protected:
		template <class L, class EH>
		void addMasterHandler(const EH& handler) {
		    m_wMaster.template addHandler<L>(handler);
		}

		template <class Win, class ... WinArgs>
		Win* addWindow(const std::string& name, WinArgs&&... args) {
			auto result = m_wMaster.addWidget(std::make_unique<Win>(m_wMaster, std::forward<WinArgs>(args)...));
			auto t = reinterpret_cast<Win*>(result);
			m_windowAnnuary[name] = t;
			t->template addHeadHandler<ClickEventListener>([&](Widget* tthis, ClickEvent& e) {
				windowSorter(tthis, e);
			});
			return result;
		}

		void removeWindow(const std::string& name) {
			auto wPtr = m_windowAnnuary[name];
			m_wMaster.removeWidget(static_cast<BaseHandledWidget*>(wPtr));
			m_windowAnnuary.erase(name);
		}

		void pushWindowToFront(Widget* w);
		Widget* addTopWidget(std::unique_ptr<Widget>& w);


		Widget* getWindow(const std::string& name) {
			return m_windowAnnuary[name];
		}

		unsigned int getMaxHeight() const;
		unsigned int getMaxWidth() const;
	};
}
