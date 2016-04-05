#pragma once
#include <unordered_set>
#include "../Rectangle.h"
#include "../../ECS/System.h"
#include "../../Physic/PositionComponent.h"
#include "../CameraFocusedComponent.h"

namespace ska {
	template <class T>
	struct Point;

	class CameraSystem : public System<std::unordered_set<ska::EntityId>, PositionComponent, CameraFocusedComponent> {
	public:
		CameraSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH);

		void screenResized(const unsigned int screenW, const unsigned int screenH);
		void worldResized(const unsigned int worldW, const unsigned int worldH);

		virtual const Rectangle* getDisplay() const;
		virtual ~CameraSystem();

	protected:
		void focusOn(Rectangle& pos, EntityId* optionalEntityId);

		unsigned int m_worldW, m_worldH;
		Rectangle* m_pos;
		Rectangle m_cameraRect;
	};
}
