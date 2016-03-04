#pragma once
#include "Component.h"
#include "../../Graphic/Texture.h"

namespace ska {
	class PositionComponent;
	class GraphicComponent : public  Component {
	public:
		GraphicComponent();
		GraphicComponent(const PositionComponent* pos);

		const PositionComponent* getPositionComponent() const;
		Texture& getSprite();

		virtual ~GraphicComponent();

	private:
		Texture m_sprite;
		const PositionComponent* m_pos;

	};
	typedef std::unique_ptr<GraphicComponent> GraphicComponentPtr;
}