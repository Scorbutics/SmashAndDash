#pragma once

namespace ska {
	class DrawableContainer;
	class HasGraphic {
	public:
		HasGraphic() {}
		virtual ~HasGraphic() {}

		virtual void graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables) = 0;
	};
}
