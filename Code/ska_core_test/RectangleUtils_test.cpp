#include <doctest.h>
#include "Utils/RectangleUtils.h"

TEST_CASE("[RectangleUtils]Position dans la box") {

    ska::Rectangle box {0, 4, 14, 24};

    SUBCASE("Cas interieur") {
        ska::Point<int> p(10, 10);
        CHECK(ska::RectangleUtils::isPositionInBox(p, box));
    }

    SUBCASE("Cas exterieur") {
        ska::Point<int> p(100, 100);
        CHECK(!ska::RectangleUtils::isPositionInBox(p, box));
    }

    SUBCASE("Cas aux limites x sup") {
        ska::Point<int> p(13, 17);
        CHECK(ska::RectangleUtils::isPositionInBox(p, box));
        p.x++;
        CHECK(!ska::RectangleUtils::isPositionInBox(p, box));
    }

    SUBCASE("Cas aux limites x inf") {
        ska::Point<int> p(0, 17);
        CHECK(ska::RectangleUtils::isPositionInBox(p, box));
        p.x--;
        CHECK(!ska::RectangleUtils::isPositionInBox(p, box));
    }

    SUBCASE("Cas aux limites y sup") {
        ska::Point<int> p(10, 27);
        CHECK(ska::RectangleUtils::isPositionInBox(p, box));
        p.y++;
        CHECK(!ska::RectangleUtils::isPositionInBox(p, box));
    }

    SUBCASE("Cas aux limites y inf") {
        ska::Point<int> p(10, 4);
        CHECK(ska::RectangleUtils::isPositionInBox(p, box));
        p.y--;
        CHECK(!ska::RectangleUtils::isPositionInBox(p, box));
    }
}
