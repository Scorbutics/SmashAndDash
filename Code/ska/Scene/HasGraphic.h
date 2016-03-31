#pragma once
#include <queue>

namespace ska {
	class DrawableContainer;
	class HasGraphic {
	public:
		HasGraphic() {}
		virtual ~HasGraphic() {}

		virtual void graphicUpdate(DrawableContainer& drawables) = 0;
	};
}