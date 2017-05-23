#include <doctest.h>
#include "GUI/Events/ClickEventListener.h"
#include "GUI/GUI.h"
#include "WindowsUtil.h"
#include "Inputs/InputContextManager.h"
#include "InputContextTest.h"
#include "MoveableWindowTest.h"

template <class T>
void printVector(std::vector<T>& v) {
    for(const auto& e : v) {
        std::cout << "[" << e << "]";
    }
    std::cout << std::endl;
}

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

void reportMouseLastPos() {
    auto ict = GetICT();
	ict->mouseKeys.setMouseLastPos(ict->mouseKeys.getMousePos());
}

void moveMouse(int x, int y) {
	SDL_Event event;
	event.motion.x = x;
	event.motion.y = y;
	event.motion.xrel = x - GetICT()->mouseKeys.getMousePos().x;
	event.motion.yrel = y - GetICT()->mouseKeys.getMousePos().y;

	GetICT()->mouseKeys.setMousePos(event);
}

void clickMouseLeft(bool d) {
    auto ict = GetICT();
    ict->mouseKeys.setMouseState(0, d);
}

void basicRefreshLoop(ska::GUI& gui) {
    auto& icm = GetICM();
    icm.refresh();
    gui.refresh();
    reportMouseLastPos();
}

TEST_CASE("[GUI]Deplacement d'une fenetre (focusable)") {
    SubGUIMock gui;

	auto& window = gui.mockAddFocusableWindow<MoveableWindowTest<>>("noname", ska::Rectangle{ 0, 0, 120, 40 }, "nostyle");

	//Sets the mouse pos
	moveMouse(21, 4);

	//Mouse left click down
	clickMouseLeft(true);

	//refresh
	basicRefreshLoop(gui);

	//Move the mouse
	unsigned int xMax = 52;
	for (unsigned int x = 21; x < xMax; x += 2) {
		moveMouse(x, 4);
        basicRefreshLoop(gui);
	}

	CHECK(window.getBox().x == 30);
}

TEST_CASE("[GUI]Deplacement d'une fenetre (non focusable)") {
    SubGUIMock gui;

	auto& window = gui.mockAddWindow<MoveableWindowTest<>>("noname", ska::Rectangle{ 0, 0, 120, 40 }, "nostyle");

	//Sets the mouse pos
	moveMouse(21, 4);

	//Mouse left click down
	clickMouseLeft(true);

	//refresh
	basicRefreshLoop(gui);

	//Move the mouse
	unsigned int xMax = 52;
	for (unsigned int x = 21; x < xMax; x += 2) {
		moveMouse(x, 4);
        basicRefreshLoop(gui);
	}

	CHECK(window.getBox().x == 30);
}

TEST_CASE("[GUI]Evenements : Mouse ENTER, OVER, RELEASE") {
	SubGUIMock gui;

	auto& window = gui.mockAddWindow<MoveableWindowTest<>>("noname", ska::Rectangle{ 5, 4, 120, 40 }, "nostyle");

	std::vector<ska::MouseEventType> states;
	std::vector<ska::MouseEventType> expected;
	expected.push_back(ska::MouseEventType::MOUSE_ENTER);
	expected.push_back(ska::MouseEventType::MOUSE_OVER);
	expected.push_back(ska::MouseEventType::MOUSE_OVER);
	expected.push_back(ska::MouseEventType::MOUSE_RELEASE);

	window.addHandler<ska::HoverEventListener>([&] (ska::Widget* tthis, ska::HoverEvent& ce) {
        states.push_back(ce.getState());
    });

    //Sets the mouse pos IN
	moveMouse(21, 5);
	basicRefreshLoop(gui);

	CHECK(states.size() == 2);
	CHECK(states[0] == expected[0]);
	CHECK(states[1] == expected[1]);

	//Sets the mouse pos OUT
	moveMouse(1, 1);
	basicRefreshLoop(gui);

	//It's the last mouse pos which is used to OVER event, so another one is triggered
    CHECK(states.size() == 3);
    CHECK(states[2] == expected[2]);

    moveMouse(0, 1);

    //Now we should also have mouse OUT event
    basicRefreshLoop(gui);
    moveMouse(0, 0);
    basicRefreshLoop(gui);

	printVector(states);
	printVector(expected);

	CHECK(states == expected);
}

TEST_CASE("[GUI]Changement de focus des fenetres") {
	SubGUIMock gui;

	auto& window = gui.mockAddWindow<MoveableWindowTest<>>("noname", ska::Rectangle{ 0, 0, 120, 40 }, "nostyle");
	auto& window2 = gui.mockAddWindow<MoveableWindowTest<>>("noname2", ska::Rectangle{ 150, 0, 100, 40 }, "nostyle");

	auto p1 = window.getPriority();
	auto p2 = window2.getPriority();

	CHECK(p1 > p2);



}
