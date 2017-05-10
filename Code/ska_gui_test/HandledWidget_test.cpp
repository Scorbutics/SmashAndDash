#include <doctest.h>
#include "./GUI/Components/HandledWidget.h"
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


	ska::Point<int> p(0, 0);
	ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, p);
	t.directNotify(ce);

	CHECK(clicked);
}
