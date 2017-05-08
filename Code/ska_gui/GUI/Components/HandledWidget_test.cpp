#include "doctest.h"
#include "HandledWidget.h"
#include "./GUI/Events/ClickEventListener.h"
#include "./GUI/Events/ClickEvent.h"

TEST_CASE("testing the HandledWidget class") {
	struct HandledWidgetTest : public ska::HandledWidget<ska::ClickEventListener> {
		void display() const override {
		}
	};

	HandledWidgetTest t;
	auto clicked = false;
	t.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
		clicked = true;
	});


	ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, ska::Point<int>(0, 0));
	t.directNotify(ce);

	CHECK(clicked);
}