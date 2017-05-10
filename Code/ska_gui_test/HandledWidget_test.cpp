#include <vector>
#include <doctest.h>
#include "./GUI/Components/HandledWidget.h"
#include "./GUI/Events/ClickEventListener.h"
#include "./GUI/Events/ClickEvent.h"

template <class ... EL>
struct HandledWidgetTest : public ska::HandledWidget<EL...> {
    void display() const override {
    }
};


TEST_CASE("[HandledWidget]Position par defaut") {
    HandledWidgetTest<ska::ClickEventListener> t;
    CHECK(t.getAbsolutePosition() == ska::Point<int>(0, 0));
    CHECK(t.getRelativePosition() == ska::Point<int>(0, 0));
}

TEST_CASE("[HandledWidget]Dimensions par defaut") {
    HandledWidgetTest<ska::ClickEventListener> t;
    CHECK(t.getBox().w == 0);
    CHECK(t.getBox().h == 0);
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
