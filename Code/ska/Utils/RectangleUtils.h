#pragma once

#include "../Graphic/Rectangle.h"

namespace ska {
	class RectangleUtils
	{
	public:
		static unsigned int distanceSquared(const ska::Rectangle* pos1, const ska::Rectangle* pos2);
		static bool isPositionInBox(const ska::Rectangle *pos, const ska::Rectangle *box);
		static ska::Rectangle posToCenterPicture(const ska::Rectangle *imageToCenter, const ska::Rectangle *imageBackground);
		static bool collisionBoxABoxB(const ska::Rectangle rectA, const ska::Rectangle rectB);
		static int getDirectionFromPos(const ska::Rectangle *posHero, const ska::Rectangle *mousePos);
		~RectangleUtils();

	private:
		RectangleUtils();

	};
}
