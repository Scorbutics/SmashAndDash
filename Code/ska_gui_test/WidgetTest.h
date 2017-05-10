#pragma once
#include "GUI/Components/Widget.h"

struct WidgetTest : public ska::Widget {
	WidgetTest() {
	}

	WidgetTest(ska::Widget& parent) : ska::Widget(parent) {
	}

	WidgetTest(ska::Widget& parent, ska::Point<int>& p) : ska::Widget(parent, p) {
	}

	void display() const override {
	}
};
