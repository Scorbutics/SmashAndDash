#pragma once
#include "../../ska/ECS/Component.h"
#include "../../ska/Graphic/Texture.h"

class GraphicComponent : public ska::Component {
public:
	ska::Texture sprite;
	void operator=(const GraphicComponent& gc) {
		sprite = gc.sprite;
	}

};
