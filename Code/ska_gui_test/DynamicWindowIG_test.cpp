#include <doctest.h>
#include "Point.h"
#include "WindowsUtil.h"
#include "GUI/Events/ClickEventListener.h"
#include "GUI/Windows/DynamicWindowIG.h"
#include "Core/Window.h"

template<class ... T>
class DynamicWindowTest : public ska::DynamicWindowIG<T...> {
public:
	DynamicWindowTest(ska::Widget& parent, const ska::Rectangle& box, const std::string& styleName) : ska::DynamicWindowIG<T...>(parent, box, styleName) {
	}

	DynamicWindowTest(ska::MouseObservable* mo, ska::KeyObservable* ko, const ska::Rectangle& box, const std::string& styleName) : ska::DynamicWindowIG<T...>(mo, ko, box, styleName) {
	}

	virtual void display() const override {
		ska::DynamicWindowIG<T...>::display();
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


TEST_CASE("[DynamicWindow] Observable : clic") {
	GetWindow();
	ObservableTest obs;
	DynamicWindowTest<> dw(&obs, &obs, ska::Rectangle{ 0, 0, 10, 10 }, "nostyle");

	auto clicked = false;
	dw.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
		clicked = true;
	});

	ska::Point<int> cePoint(0, 0);
	ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, cePoint);
	obs.ska::ClickObservable::notifyObservers(ce);

	CHECK(clicked);
}
