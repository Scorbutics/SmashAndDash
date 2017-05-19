#include <doctest.h>
#include "Point.h"
#include "WindowsUtil.h"
#include "GUI/Events/ClickEventListener.h"
#include "GUI/Windows/MoveableWindow.h"
#include "Core/Window.h"

template<class ... T>
class MoveableWindowTest : public ska::MoveableWindow<T...> {
public:
	MoveableWindowTest(ska::Widget& parent, const ska::Rectangle& box, const std::string& styleName) : ska::MoveableWindow<T...>(parent, box, styleName) {
	}

	MoveableWindowTest(ska::MouseObservable& mo, ska::KeyObservable& ko, const ska::Rectangle& box, const std::string& styleName) : ska::MoveableWindow<T...>(mo, ko, box, styleName) {
	}

	virtual void display() const override {
		ska::MoveableWindow<T...>::display();
		m_callbackDisplay();
	}

	void whenDisplayed(std::function<void(void)> callback) {
		m_callbackDisplay = callback;
	}

private:
	std::function<void(void)> m_callbackDisplay;
};

class ObservableTest : public ska::MouseObservable, public ska::KeyObservable {

};


TEST_CASE("[MoveableWindow]") {
	GetWindow();
	ObservableTest obs;
	MoveableWindowTest<> dw(obs, obs, ska::Rectangle{ 0, 0, 10, 10 }, "nostyle");
}
