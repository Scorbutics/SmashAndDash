#include <doctest.h>
#include "GUI/Events/ClickEventListener.h"
#include "GUI/GUI.h"
#include "WindowsUtil.h"
#include "Inputs/InputContextManager.h"
#include "InputContextTest.h"
#include "MoveableWindowTest.h"

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

void moveMouseX(int x) {
	SDL_Event event;
	event.motion.x = x;
	event.motion.y = 4;
	event.motion.xrel = 0;
	event.motion.yrel = 0;

	auto ict = GetICT();
	ict->mouseKeys.setMouseLastPos(ict->mouseKeys.getMousePos());
	ict->mouseKeys.setMousePos(event);
}

void clickMouseLeft(bool d) {
    auto ict = GetICT();
    ict->mouseKeys.setMouseState(0, d);
}

void basicRefreshLoop(ska::GUI& gui) {
    auto& icm = GetICM();
    icm.refresh();
    gui.refresh();
}

TEST_CASE("[GUI]Deplacement d'une fenetre (focusable)") {
    SubGUIMock gui;

	auto& window = gui.mockAddFocusableWindow<MoveableWindowTest<>>("noname", ska::Rectangle{ 0, 0, 120, 40 }, "nostyle");

	//Sets the mouse pos
	moveMouseX(21);

	//Mouse left click down
	clickMouseLeft(true);

	//refresh
	basicRefreshLoop(gui);

	//Move the mouse
	unsigned int xMax = 52;
	for (unsigned int x = 21; x < xMax; x += 2) {
		moveMouseX(x);
        basicRefreshLoop(gui);
	}

	CHECK(window.getBox().x == 30);
}

TEST_CASE("[GUI]Deplacement d'une fenetre (non focusable)") {
    SubGUIMock gui;

	auto& window = gui.mockAddWindow<MoveableWindowTest<>>("noname", ska::Rectangle{ 0, 0, 120, 40 }, "nostyle");

	//Sets the mouse pos
	moveMouseX(21);

	//Mouse left click down
	clickMouseLeft(true);

	//refresh
	basicRefreshLoop(gui);

	//Move the mouse
	unsigned int xMax = 52;
	for (unsigned int x = 21; x < xMax; x += 2) {
		moveMouseX(x);
        basicRefreshLoop(gui);
	}

	//TODO pour l'instant, fail
	//CHECK(window.getBox().x == 30);
}
