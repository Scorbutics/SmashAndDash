#pragma once
#include "GUI/Windows/MoveableWindow.h"

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
