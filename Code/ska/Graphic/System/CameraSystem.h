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

		const Rectangle* getDisplay() const;
		virtual ~CameraSystem();

	protected:
		void focusOn(Point<int>& pos, EntityId* optionalEntityId);

		unsigned int m_worldW, m_worldH;
		Point<int>* m_pos;
		Rectangle m_cameraRect;
	};
}
