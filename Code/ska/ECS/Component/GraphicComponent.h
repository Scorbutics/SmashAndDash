#pragma once
#include "Component.h"
#include "../../Graphic/Texture.h"

namespace ska {
	class PositionComponent;
	class GraphicComponent : public  Component {
	public:
		GraphicComponent();

		Texture& getSprite();
		virtual ~GraphicComponent();

	private:
		Texture m_sprite;

	};
	typedef std::unique_ptr<GraphicComponent> GraphicComponentPtr;
}