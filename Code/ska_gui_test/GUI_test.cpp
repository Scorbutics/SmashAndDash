#include <doctest.h>
#include "GUI/Events/ClickEventListener.h"
#include "GUI/GUI.h"
#include "WindowsUtil.h"
#include "Inputs/InputContextManager.h"

template<class ... T>
class StaticWindowTest : public ska::WindowIG<T...> {
public:
	StaticWindowTest(ska::Widget& parent, const ska::Rectangle& box, const std::string& styleName) : ska::WindowIG<T...>(parent, box, styleName) {
	}

	StaticWindowTest(const ska::Rectangle& box, const std::string& styleName) : ska::WindowIG<T...>(box, styleName) {
	}

	virtual void display() const override {
	    ska::WindowIG<T...>::display();
	    m_callbackDisplay();
	}

	void whenDisplayed(std::function<void(void)> callback) {
	    m_callbackDisplay = callback;
	}

private:
    std::function<void(void)> m_callbackDisplay;
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

    template <class Win, class ... WinArgs>
	Win& mockAddFocusableWindow(const std::string& name, WinArgs&&... args) {
		return addFocusableWindow<Win>(name, std::forward<WinArgs>(args)...);
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

TEST_CASE("[GUI]Affichage fenetre") {
    SubGUIMock gui;
    auto displayed = false;

	gui.mockAddWindow<StaticWindowTest<ska::ClickEventListener>>("noname", ska::Rectangle{ 0 }, "nostyle")
	.whenDisplayed([&]() {
        displayed = true;
    });

    gui.display();

	CHECK(displayed);
}

TEST_CASE("[GUI]Affichage fenetre") {
    SubGUIMock gui;
    auto displayed = false;

	gui.mockAddWindow<StaticWindowTest<ska::ClickEventListener>>("noname", ska::Rectangle{ 0 }, "nostyle")
	.whenDisplayed([&]() {
        displayed = true;
    });

    gui.display();

	CHECK(displayed);
}
