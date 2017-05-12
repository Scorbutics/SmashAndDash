#include <doctest.h>
#include "GUI/Components/WidgetPanel.h"
#include "GUI/Events/ClickEventListener.h"
#include "GUI/Events/KeyEventListener.h"
#include "GUI/Events/KeyEvent.h"
#include "HandledWidgetTest.h"
#include "WidgetTest.h"

TEST_CASE("[WidgetPanel]Ajout de widgets") {
	ska::WidgetPanel<ska::ClickEventListener> wp;
	CHECK(wp.backWidget() == nullptr);
	CHECK(wp.backAddedWidget() == nullptr);

	auto& hwt = wp.addWidget<HandledWidgetTest<ska::ClickEventListener>>();
	CHECK(wp.backWidget() == &hwt);
	CHECK(wp.backAddedWidget() == &hwt);

	auto& wt = wp.addWidget<WidgetTest>();
	CHECK(wp.backWidget() == &wt);
	CHECK(wp.backAddedWidget() == &wt);
}


TEST_CASE("[WidgetPanel]Vidage de widgets") {
	ska::WidgetPanel<ska::ClickEventListener> wp;

	wp.addWidget<HandledWidgetTest<ska::ClickEventListener>>();
	wp.addWidget<WidgetTest>();

	wp.clear();
	CHECK(wp.backWidget() == nullptr);
	CHECK(wp.backAddedWidget() == nullptr);
}

TEST_CASE("[WidgetPanel]Propagation d'evenements dans les widgets contenus") {
	ska::WidgetPanel<ska::ClickEventListener> wp;

	auto& hwt = wp.addWidget<HandledWidgetTest<ska::ClickEventListener>>();
	auto& wt = wp.addWidget<WidgetTest>();

	ska::Point<int> pClickEvent(0, 0);
	ska::ClickEvent ce(ska::MOUSE_CLICK, pClickEvent);

	auto widgetPanelNotified = false;
	wp.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
		widgetPanelNotified = true;
	});

	auto widgetNotified = false;
	hwt.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
		widgetNotified = true;
	});


	SUBCASE("Notification sur WidgetPanel") {
		CHECK(wp.notify(ce));
		CHECK(widgetPanelNotified);
		CHECK(widgetNotified);
	}

	SUBCASE("Notification sur HandledWidget") {
		CHECK(hwt.notify(ce));
		CHECK(!widgetPanelNotified);
		CHECK(widgetNotified);
	}

	SUBCASE("Notification sur Widget") {
		CHECK(!wt.notify(ce));
		CHECK(!widgetPanelNotified);
		CHECK(!widgetNotified);
	}
}

TEST_CASE("[WidgetPanel]Propagation d'evenements avec blocage") {
	ska::WidgetPanel<ska::ClickEventListener> wp;

	auto& hwt = wp.addWidget<HandledWidgetTest<ska::ClickEventListener>>();
	auto& hwt2 = wp.addWidget<HandledWidgetTest<ska::ClickEventListener>>();
	wp.addWidget<WidgetTest>();

	ska::Point<int> pClickEvent(0, 0);
	ska::ClickEvent ce(ska::MOUSE_CLICK, pClickEvent);

    auto widgetPanelNotified = false;
    wp.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
        widgetPanelNotified = true;
    });

    auto hwt2Notified = false;
    hwt2.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
        hwt2Notified = true;
    });

	SUBCASE("Stop callback") {
        auto widgetNotified = false;
        hwt.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
            e.stopPropagation(ska::StopType::STOP_CALLBACK);
            widgetNotified = true;
        });

		CHECK(wp.notify(ce));
		CHECK(widgetNotified);

		//notifié
		CHECK(hwt2Notified);

		//notification du conteneur
		CHECK(widgetPanelNotified);
	}

    SUBCASE("Stop widget") {
        auto widgetNotified = false;
        hwt.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
            e.stopPropagation(ska::StopType::STOP_WIDGET);
            widgetNotified = true;
        });

		CHECK(wp.notify(ce));
		CHECK(widgetNotified);

		//stoppé
		CHECK(!hwt2Notified);

		//le conteneur est quand même notifié
		CHECK(widgetPanelNotified);
	}
}

TEST_CASE("[WidgetPanel]Propagation d'evenements avec types de handlers differents") {
	ska::WidgetPanel<ska::ClickEventListener> wp;

	auto& hwt = wp.addWidget<HandledWidgetTest<ska::KeyEventListener>>();
	auto& hwt2 = wp.addWidget<HandledWidgetTest<ska::ClickEventListener>>();
	wp.addWidget<HandledWidgetTest<>>();
	wp.addWidget<WidgetTest>();

	ska::Point<int> pClickEvent(0, 0);
	ska::ClickEvent ce(ska::MOUSE_CLICK, pClickEvent);

    auto widgetPanelNotified = false;
    wp.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
        widgetPanelNotified = true;
    });

    auto hwtNotified = false;
    hwt.addHandler<ska::KeyEventListener>([&](ska::Widget* tthis, ska::KeyEvent& e) {
        hwtNotified = true;
    });

    auto hwt2Notified = false;
    hwt2.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
        hwt2Notified = true;
    });

    CHECK(wp.notify(ce));
    CHECK(!hwtNotified);
    CHECK(hwt2Notified);
    CHECK(widgetPanelNotified);

}
