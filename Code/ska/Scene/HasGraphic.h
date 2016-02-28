#pragma once
#include "../Graphic/Draw/DrawableContainer.h"
#include <queue>

namespace ska {
	class HasGraphic {
	public:
		HasGraphic() {}
		virtual ~HasGraphic() {}

		virtual void graphicUpdate(DrawableContainer& drawables) = 0;
	};
}