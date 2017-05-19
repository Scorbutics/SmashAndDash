#include <doctest.h>
#include "GUI/Components/WidgetPanel.h"
#include "GUI/Events/ClickEventListener.h"
#include "GUI/Events/HoverEventListener.h"
#include "GUI/Events/KeyEventListener.h"
#include "GUI/Events/KeyEvent.h"
#include "HandledWidgetTest.h"
#include "WidgetTest.h"

unsigned int DisplayCounter::displayCounter = 0;
std::vector<const DisplayCounter*> DisplayCounter::instances;

TEST_CASE("[WidgetPanel]Ajout de widgets") {
	ska::WidgetPanel<ska::ClickEventListener> wp;
	CHECK(wp.backWidget() == nullptr);
	CHECK(wp.backAddedWidget() == nullptr);

	auto& hwt = wp.addWidget<HandledWidgetTest<ska::ClickEventListener>>();
	CHECK(wp.backWidget() == &hwt);
	CHECK(wp.backAddedWidget() == &hwt);
}


TEST_CASE("[WidgetPanel]Vidage de widgets") {
	ska::WidgetPanel<ska::ClickEventListener> wp;

	wp.addWidget<HandledWidgetTest<ska::ClickEventListener>>();
	wp.addWidget<WidgetTest>();

	wp.clear();
	CHECK(wp.backWidget() == nullptr);
	CHECK(wp.backAddedWidget() == nullptr);
}

TEST_CASE("[WidgetPanel]Visibilite des Widgets") {
	ska::WidgetPanel<ska::ClickEventListener> wp;

	wp.addWidget<HandledWidgetTest<ska::ClickEventListener>>();
	wp.addWidget<HandledWidgetTest<ska::ClickEventListener>>();
	wp.addWidget<WidgetTest>();
	DisplayCounter::reset();

	wp.display();
	CHECK(DisplayCounter::getDisplayCounter() == 2);

	DisplayCounter::reset();
	wp.showWidgets(false);

	wp.display();
	CHECK(DisplayCounter::getDisplayCounter() == 0);
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

	SUBCASE("Stop callback") {
		auto hwt2Notified = false;
		hwt2.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
			hwt2Notified = true;
		});

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
		auto hwt2Notified = false;
		hwt2.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
			e.stopPropagation(ska::StopType::STOP_WIDGET);
			hwt2Notified = true;
		});

        auto widgetNotified = false;
        hwt.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& e) {
            e.stopPropagation(ska::StopType::STOP_WIDGET);
            widgetNotified = true;
        });

		CHECK(wp.notify(ce));
		CHECK(hwt2Notified);

		//stoppé
		CHECK(!widgetNotified);

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

TEST_CASE("[WidgetPanel]Propagation d'evenements avec visibilites differentes") {
	ska::WidgetPanel<ska::HoverEventListener> wp;

	auto& hwt = wp.addWidget<HandledWidgetTest<ska::KeyEventListener>>();
	auto& hwt2 = wp.addWidget<HandledWidgetTest<ska::HoverEventListener>>();
	auto& hwt3 = wp.addWidget<HandledWidgetTest<ska::HoverEventListener>>();
	wp.addWidget<WidgetTest>();

	ska::Point<int> pHoverEvent(0, 0);
	ska::HoverEvent he(ska::MOUSE_ENTER, pHoverEvent, pHoverEvent, pHoverEvent);

	hwt.show(false);

	std::vector<int> order;
	std::vector<int> expectedOrder;
	expectedOrder.push_back(3);
	expectedOrder.push_back(2);
	expectedOrder.push_back(0);

	wp.addHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
		order.push_back(0);
	});

	hwt.addHandler<ska::KeyEventListener>([&](ska::Widget* tthis, ska::KeyEvent& e) {
		order.push_back(1);
	});

	hwt2.addHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
		order.push_back(2);
	});

	hwt3.addHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
		order.push_back(3);
	});

	CHECK(wp.notify(he));
	CHECK(order == expectedOrder);

}

TEST_CASE("[WidgetPanel]Affichage par priorite") {
	ska::WidgetPanel<ska::HoverEventListener> wp;

	auto& hwt4 = wp.addWidget<HandledWidgetTest<ska::HoverEventListener>>();
	auto& hwt2 = wp.addWidget<HandledWidgetTest<ska::HoverEventListener>>();
	auto& hwt = wp.addWidget<HandledWidgetTest<ska::HoverEventListener>>();
	auto& hwt3 = wp.addWidget<HandledWidgetTest<ska::HoverEventListener>>();


	ska::Point<int> pHoverEvent(0, 0);
	ska::HoverEvent he(ska::MOUSE_ENTER, pHoverEvent, pHoverEvent, pHoverEvent);

	std::vector<const DisplayCounter*> expectedOrder;

	hwt3.show(false);

	hwt4.setPriority(410);
	hwt.setPriority(287);
	hwt3.setPriority(20);
	hwt2.setPriority(14);


	SUBCASE("Avec un resort avant") {
		DisplayCounter::reset();
		//Trié par ordre de priorité décroissant
		expectedOrder.push_back(&hwt4);
		expectedOrder.push_back(&hwt);
		expectedOrder.push_back(&hwt2);

		//A la fin car invisible
		expectedOrder.push_back(&hwt3);

		wp.resort();

		wp.display();
		CHECK(DisplayCounter::getInstances() == expectedOrder);


		std::vector<int> order;
		std::vector<int> expectedEventOrder;
		expectedEventOrder.push_back(2);
		expectedEventOrder.push_back(1);
		expectedEventOrder.push_back(4);

		hwt.addHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
			order.push_back(1);
		});

		hwt2.addHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
			order.push_back(2);
		});

		hwt3.addHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
			order.push_back(3);
		});

		hwt4.addHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
			order.push_back(4);
		});

		//On vérifie que l'ordre de notification reste correct (par ordre de priorité inversé par rapport à l'affichage !)
		//en effet, l'ordre de déclenchement des évènements doit être de la fenêtre au premier plan
		//(donc la dernière affichée) vers la fenêtre au dernier plan
		wp.notify(he);

		CHECK(order == expectedEventOrder);
	}

	SUBCASE("Sans resort"){
		DisplayCounter::reset();

		//Trié par ordre d'ajout
		expectedOrder.push_back(&hwt4);
		expectedOrder.push_back(&hwt2);
		expectedOrder.push_back(&hwt);
		expectedOrder.push_back(&hwt3);

		wp.display();
		CHECK(DisplayCounter::getInstances() == expectedOrder);
	}

}

TEST_CASE("[WidgetPanel]Evenements par priorite") {
	ska::WidgetPanel<ska::HoverEventListener> wp;

	auto& hwt = wp.addWidget<HandledWidgetTest<ska::HoverEventListener>>();
	auto& hwt2 = wp.addWidget<HandledWidgetTest<ska::HoverEventListener>>();
	auto& hwt3 = wp.addWidget<HandledWidgetTest<ska::HoverEventListener>>();

	ska::Point<int> pHoverEvent(0, 0);
	ska::HoverEvent he(ska::MOUSE_ENTER, pHoverEvent, pHoverEvent, pHoverEvent);

	std::vector<int> order;
	std::vector<int> expectedOrder;
	expectedOrder.push_back(3);
	expectedOrder.push_back(2);
	expectedOrder.push_back(1);
	expectedOrder.push_back(0);

	hwt.setPriority(287);
	hwt2.setPriority(14);
	hwt3.setPriority(2);

	wp.addHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
		order.push_back(0);
	});

	hwt.addHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
		order.push_back(1);
	});

	hwt2.addHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
		order.push_back(2);
	});

	hwt3.addHandler<ska::HoverEventListener>([&](ska::Widget* tthis, ska::HoverEvent& e) {
		order.push_back(3);
	});

	CHECK(wp.notify(he));
	CHECK(order == expectedOrder);

}
