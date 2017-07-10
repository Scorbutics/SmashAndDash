#pragma once

#include "../Rectangle.h"
#include "../Point.h"

namespace ska {
	class RectangleUtils {
	public:
		template <class T>
		static T distanceSquared(const Point<T>& pos1, const Point<T>& pos2) {
			return (((pos1.x - pos2.x) * (pos1.x - pos2.x)) + ((pos1.y - pos2.y) * (pos1.y - pos2.y)));	
		}

		template <class T>
		static bool isPositionInBox(const Point<T> &pos, const Rectangle &box) {
			return((pos.x >= box.x) && (pos.x < box.x + box.w) && (pos.y >= box.y) && (pos.y < box.y + box.h));
		}

		static Rectangle intersect(const Rectangle& r1, const Rectangle& r2);
		static Rectangle posToCenterPicture(const Rectangle& imageToCenter, const Rectangle& imageBackground);
		static bool collisionBoxABoxB(const Rectangle& rectA, const Rectangle& rectB);
		static int getDirectionFromPos(const Point<int>& posHero, const Point<int>& mousePos);
		~RectangleUtils();

	private:
		RectangleUtils();

	};
}
