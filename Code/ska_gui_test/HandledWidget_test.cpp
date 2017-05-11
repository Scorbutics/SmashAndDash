#include <vector>
#include <doctest.h>
#include "./GUI/Components/HandledWidget.h"
#include "./GUI/Events/ClickEventListener.h"
#include "./GUI/Events/ClickEvent.h"
#include "HandledWidgetTest.h"


TEST_CASE("[HandledWidget]Position par defaut") {
    HandledWidgetTest<ska::ClickEventListener> t;
    CHECK(t.getAbsolutePosition() == ska::Point<int>(0, 0));
    CHECK(t.getRelativePosition() == ska::Point<int>(0, 0));
}

TEST_CASE("[HandledWidget]Dimensions par defaut") {
    HandledWidgetTest<ska::ClickEventListener> t;
    CHECK(t.getBox().w == 1);
    CHECK(t.getBox().h == 1);
}

TEST_CASE("[HandledWidget]Evenement de clic direct") {
    HandledWidgetTest<ska::ClickEventListener> t;
    auto clicked = false;
    t.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
        clicked = true;
    });

    ska::Point<int> p(0, 0);
    ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, p);
    t.directNotify(ce);

    CHECK(clicked);
}

TEST_CASE("[HandledWidget]Parent par defaut") {
    HandledWidgetTest<ska::ClickEventListener> t;
    CHECK(t.getParent() == nullptr);
}

TEST_CASE("[HandledWidget]Handlers ordonnes par ordre d'ajout") {
    HandledWidgetTest<ska::ClickEventListener> t;
    std::vector<int> eventList;
    std::vector<int> expected;
    expected.push_back(2);
    expected.push_back(0);
    expected.push_back(1);

    t.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
        eventList.push_back(0);
    });

    t.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
        eventList.push_back(1);
    });

    t.addHeadHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
        eventList.push_back(2);
    });

    ska::Point<int> p(0, 0);
    ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, p);
    t.directNotify(ce);

    CHECK(eventList == expected);
}

TEST_CASE("[HandledWidget]Propagation d'evenements stoppee (callback)") {
    HandledWidgetTest<ska::ClickEventListener> t;

    t.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
        ce.stopPropagation(ska::StopType::STOP_CALLBACK);
    });

    auto triggered = false;
    t.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
        triggered = true;
    });

    ska::Point<int> p(0, 0);
    ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, p);
    t.directNotify(ce);

    CHECK(!triggered);
}

TEST_CASE("[HandledWidget]Propagation d'evenements stoppee (widget)") {
    HandledWidgetTest<ska::ClickEventListener> t;

    t.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
        ce.stopPropagation(ska::StopType::STOP_WIDGET);
    });

    auto triggered = false;
    t.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
        triggered = true;
    });

    ska::Point<int> p(0, 0);
    ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, p);
    t.directNotify(ce);

    CHECK(triggered);
}

TEST_CASE("[HandledWidget]Priorite d'affichage par defaut") {
    HandledWidgetTest<ska::ClickEventListener> t;
    CHECK(t.getPriority() == 0);
    CHECK(t.getPriority2D() == 0);
}

TEST_CASE("[HandledWidget]Parentes (isAParent)") {
    HandledWidgetTest<ska::ClickEventListener> child;
    HandledWidgetTest<ska::ClickEventListener> parent(child);
    HandledWidgetTest<ska::ClickEventListener> otherWidget;

    CHECK(child.isAParent(parent));
    CHECK(!parent.isAParent(child));

    CHECK(!otherWidget.isAParent(child));
    CHECK(!otherWidget.isAParent(parent));
    CHECK(!child.isAParent(otherWidget));
    CHECK(!parent.isAParent(otherWidget));
}


TEST_CASE("[HandledWidget]Propagation d'evenements avec liens de parente") {
	//Ici il faut montrer que le lien de parenté n'est PAS un lien d'appartenance,
	//c'est à dire que la notion de widget fils est seulement utile par exemple
	//pour les positions relatives et la durée de vie d'un widget
	//Il est donc normal que seulement le widget parent (car c'est sur celui-ci qu'on envoi l'évènement) qui soit notifié.

	HandledWidgetTest<ska::ClickEventListener> parent;
	ska::Point<int> pParent(0, 0);
	parent.move(pParent);
	parent.setWidth(1000);
	parent.setHeight(1000);

	HandledWidgetTest<ska::ClickEventListener> child(parent);
	ska::Point<int> pRelChild(20, 20);
	child.move(pRelChild);
	child.setWidth(100);
	child.setHeight(100);

	auto parentTriggered = false;
	parent.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
		parentTriggered = true;
	});

	auto childTriggered = false;
	child.addHandler<ska::ClickEventListener>([&](ska::Widget* tthis, ska::ClickEvent& ce) {
		childTriggered = true;
	});

	SUBCASE("directNotify dans parent") {
		//On clique à la fois dans le parent et l'enfant
		ska::Point<int> pClickEvent(1, 1);
		ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, pClickEvent);

		CHECK(parent.directNotify(ce));
		CHECK(parentTriggered);
		CHECK(!childTriggered);
	}

	SUBCASE("notify dans parent") {
		//On clique à la fois dans le parent et l'enfant
		ska::Point<int> pClickEvent(1, 1);
		ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, pClickEvent);

		CHECK(parent.notify(ce));
		CHECK(parentTriggered);
		CHECK(!childTriggered);
	}

	SUBCASE("directNotify dans enfant") {
		ska::Point<int> pClickEvent(25, 25);
		ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, pClickEvent);

		CHECK(child.directNotify(ce));
		CHECK(childTriggered);
		CHECK(!parentTriggered);
	}

	SUBCASE("notify dans enfant") {
		ska::Point<int> pClickEvent(25, 25);
		ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, pClickEvent);

		CHECK(child.notify(ce));
		CHECK(childTriggered);
		CHECK(!parentTriggered);
	}
}

TEST_CASE("[HandledWidget]Focus") {
	HandledWidgetTest<ska::ClickEventListener> widget;
	CHECK(!widget.isFocused());
	widget.focus(true);
	CHECK(widget.isFocused());
	widget.focus(false);
	CHECK(!widget.isFocused());
}

TEST_CASE("[HandledWidget]Visibilite") {
	HandledWidgetTest<ska::ClickEventListener> widget;
	CHECK(widget.isVisible());
	widget.show(false);
	CHECK(!widget.isVisible());
	widget.show(true);
	CHECK(widget.isVisible());
}

TEST_CASE("[HandledWidget]Priorite de dessin") {
	HandledWidgetTest<ska::ClickEventListener> widget;
	widget.setPriority(14);
	CHECK(widget.getPriority() == 14);
	CHECK(widget.getPriority2D() == 14);
}
