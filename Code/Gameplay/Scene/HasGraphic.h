#pragma once
#include "../../Graphic/Draw/DrawableContainer.h"
#include <queue>

class HasGraphic {
public:
	HasGraphic() {}
	virtual ~HasGraphic() {}

	virtual void graphicUpdate(DrawableContainer& drawables) = 0;
};