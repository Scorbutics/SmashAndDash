#include <doctest.h>
#include "Point.h"
#include "GUI/Events/ClickEventListener.h"
#include "HandledWidgetTest.h"

TEST_CASE("[ClickEventListener]") {

	//Pas besoin de définir la compatibilité avec ClickEventListener car le handler (callback) sera sur le listener externe
	HandledWidgetTest<> parent;
	ska::Point<int> pParent(0, 0);
	parent.move(pParent);
	parent.setWidth(1000);
	parent.setHeight(1000);

	ska::ClickEventListener cel;
	auto parentTriggered = false;
	cel.addHandler([&](ska::Widget* tthis, ska::ClickEvent& ce) {
		parentTriggered = true;
	});

	SUBCASE("notify parent") {
		ska::Point<int> pClickEvent(25, 25);
		ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, pClickEvent);

		CHECK(cel.notify(parent, ce));
		CHECK(parentTriggered);
	}

	SUBCASE("notify parent hors dimensions") {
		ska::Point<int> pClickEvent(1005, 1005);
		ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, pClickEvent);

		CHECK(!cel.notify(parent, ce));
		CHECK(!parentTriggered);
	}

}
