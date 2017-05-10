#include <doctest.h>
#include "GUI/Components/WidgetPanel.h"
#include "GUI/Events/ClickEventListener.h"
#include "HandledWidgetTest.h"
#include "WidgetTest.h"

TEST_CASE("[WidgetPanel]Ajout de widgets") {
	ska::WidgetPanel<ska::ClickEventListener> wp;
	CHECK(wp.backWidget() == nullptr);
	CHECK(wp.backAddedWidget() == nullptr);

	auto hwt = wp.addWidget(std::make_unique<HandledWidgetTest<ska::ClickEventListener>>());
	CHECK(wp.backWidget() == hwt);
	CHECK(wp.backAddedWidget() == hwt);

	auto wt = wp.addWidget(std::make_unique<WidgetTest>());
	CHECK(wp.backWidget() == wt);
	CHECK(wp.backAddedWidget() == wt);
}


TEST_CASE("[WidgetPanel]Vidage de widgets") {
	ska::WidgetPanel<ska::ClickEventListener> wp;

	wp.addWidget(std::make_unique<HandledWidgetTest<ska::ClickEventListener>>());
	wp.addWidget(std::make_unique<WidgetTest>());
	
	wp.clear();
	CHECK(wp.backWidget() == nullptr);
	CHECK(wp.backAddedWidget() == nullptr);
}

TEST_CASE("[WidgetPanel]Propagation d'evenements dans les widgets contenus") {
	ska::WidgetPanel<ska::ClickEventListener> wp;

	auto hwt = wp.addWidget(std::make_unique<HandledWidgetTest<ska::ClickEventListener>>());
	auto wt = wp.addWidget(std::make_unique<WidgetTest>());

	ska::Point<int> pClickEvent(0, 0);
	ska::ClickEvent ce(ska::MOUSE_CLICK, pClickEvent);

	auto widgetPanelNotified = false;
	wp.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
		widgetPanelNotified = true;
	});

	auto widgetNotified = false;
	hwt->addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
		widgetNotified = true;
	});


	SUBCASE("Notification sur WidgetPanel") {
		CHECK(wp.notify(ce));
		CHECK(widgetPanelNotified);
		CHECK(widgetNotified);
	}

	SUBCASE("Notification sur HandledWidget") {
		CHECK(hwt->notify(ce));
		CHECK(!widgetPanelNotified);
		CHECK(widgetNotified);
	}

	SUBCASE("Notification sur Widget") {
		CHECK(!wt->notify(ce));
		CHECK(!widgetPanelNotified);
		CHECK(!widgetNotified);
	}
}