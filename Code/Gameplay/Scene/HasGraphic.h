#pragma once
#include "../../Graphic/Drawable.h"
#include <queue>

class HasGraphic {
public:
	HasGraphic() {}
	virtual ~HasGraphic() {}

	virtual void graphicUpdate(std::priority_queue<Drawable*>& drawables) = 0;
};