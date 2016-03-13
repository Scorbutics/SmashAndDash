#pragma once

#include "../Graphic/Rectangle.h"
#include "../Graphic/Point.h"

namespace ska {
	class RectangleUtils
	{
	public:
		static unsigned int distanceSquared(const ska::Point<int>& pos1, const ska::Point<int>& pos2);
		
		template <class T>
		static bool isPositionInBox(const ska::Point<T> &pos, const ska::Rectangle &box) {
			return((pos.x >= box.x) && (pos.x <= box.x + box.w) && (pos.y >= box.y) && (pos.y <= box.y + box.h));	
		}

		static ska::Rectangle posToCenterPicture(const ska::Rectangle& imageToCenter, const ska::Rectangle& imageBackground);
		static bool collisionBoxABoxB(const ska::Rectangle& rectA, const ska::Rectangle& rectB);
		static int getDirectionFromPos(const ska::Point<int>& posHero, const ska::Point<int>& mousePos);
		~RectangleUtils();

	private:
		RectangleUtils();

	};
}
