#pragma once
#include "../Core/Component/Component.h"
#include "../../Graphic/Texture.h"

namespace ska {
	class GraphicComponent : public Component {
	public:
		Texture sprite;
		void operator=(const GraphicComponent& gc) {
			/* TODO FIXME */
		}

	};
	typedef std::unique_ptr<GraphicComponent> GraphicComponentPtr;
}