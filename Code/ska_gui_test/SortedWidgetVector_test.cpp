#include <doctest.h>
#include "Point.h"
#include "HandledWidgetTest.h"
#include "GUI/Utils/SortedWidgetVector.h"

TEST_CASE("[SortedWidgetVector]organize") {
	HandledWidgetTest<> hwt;
	HandledWidgetTest<> hwt2;
	HandledWidgetTest<> hwt3;
	HandledWidgetTest<> hwt4;
	ska::SortedWidgetVector<ska::Widget*> v;
	v.push_back(&hwt);
	v.push_back(&hwt2);
	v.push_back(&hwt3);
	v.push_back(&hwt4);

	//hwt2 devient invisible et sera donc placé à la fin.
	//hwt4 est le dernier élément et sera donc échangé avec lui
	hwt2.show(false);

	ska::SortedWidgetVector<ska::Widget*> expected;
	expected.push_back(&hwt);
	expected.push_back(&hwt4);
	expected.push_back(&hwt3);
	expected.push_back(&hwt2);

	auto i = 0U;
	for(auto& w : v) {
		v.organize(w, i);
		i++;
	}

	CHECK(expected == v);
}
