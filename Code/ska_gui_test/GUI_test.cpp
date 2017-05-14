#include <doctest.h>
#include "GUI/Events/ClickEventListener.h"
#include "GUI/GUI.h"
#include "Inputs/RawInputListener.h"
#include "Inputs/InputContextManager.h"
#include "Core/Window.h"

ska::GameEventDispatcher& GetGED () {
	static ska::GameEventDispatcher ged;
	return ged;
}

ska::BaseWindow& GetWindow() {
	static ska::Window w("title", 100, 100);
	return w;
}

ska::InputContextManager& GetICM() {
	static ska::RawInputListener ril;
	static ska::InputContextManager playerICM(ril);
	return playerICM;
}

template<class ... T>
class StaticWindowTest : public ska::WindowIG<T...> {
public:
	StaticWindowTest(ska::Widget& parent, const ska::Rectangle& box, const std::string& styleName) : ska::WindowIG<T...>(parent, box, styleName) {
	}

	StaticWindowTest(const ska::Rectangle& box, const std::string& styleName) : ska::WindowIG<T...>(box, styleName) {
	}
};

class SubGUIMock : public ska::GUI {
public:
	SubGUIMock() : ska::GUI(GetGED(), GetWindow(), GetICM()) {
	}

	virtual ~SubGUIMock() = default;

	template <class Win, class ... WinArgs>
	Win& mockAddWindow(const std::string& name, WinArgs&&... args) {
		return addWindow<Win>(name, std::forward<WinArgs>(args)...);
	}

	template <class L, class EH>
	void mockAddMasterHandler(const EH& handler) {
		addMasterHandler<L, EH>(handler);
	}

	ska::BaseHandledWidget* mockRemoveWindow(const std::string& name) {
		return removeWindow(name);
	}

	void mockPushWindowToFront(ska::Widget* w) {
		return pushWindowToFront(w);
	}

	ska::Widget* mockAddTopWidget(std::unique_ptr<ska::Widget>& w) {
		return addTopWidget(w);
	}

	ska::Widget* mockGetWindow(const std::string& name) {
		return getWindow(name);
	}

};

TEST_CASE("[GUI]") {
	
	SubGUIMock gui;
	gui.mockAddWindow<StaticWindowTest<ska::ClickEventListener>>("noname", ska::Rectangle{ 0 }, "nostyle");
	//TODO
}