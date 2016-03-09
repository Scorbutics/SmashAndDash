#pragma once
#include "../../ska/ECS/Component.h"
#include "../../ska/Graphic/Texture.h"

class GraphicComponent : public Component {
public:
	Texture sprite;
	void operator=(const GraphicComponent& gc) {
		/* TODO FIXME */
	}

};
