#pragma once
#include "GUI/Components/HandledWidget.h"

template <class ... EL>
struct HandledWidgetTest : public ska::HandledWidget<EL...> {
	HandledWidgetTest() {
	}

	HandledWidgetTest(ska::Widget& parent) : ska::HandledWidget<EL...>(parent) {
	}

	HandledWidgetTest(ska::Widget& parent, ska::Point<int>& p) : ska::HandledWidget<EL...>(parent, p) {
	}

	void display() const override {
	}
};
