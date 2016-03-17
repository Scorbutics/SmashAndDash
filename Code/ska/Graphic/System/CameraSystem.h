#pragma once
#include <unordered_set>
#include "../Rectangle.h"
#include "../../ECS/System.h"
#include "../../Physic/PositionComponent.h"
#include "../CameraFocusedComponent.h"

namespace ska {
	
	class CameraSystem : public System<std::unordered_set<ska::EntityId>, PositionComponent, CameraFocusedComponent> {
	public:
		CameraSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH);

		virtual void refresh() override;

		void screenResized(const unsigned int screenW, const unsigned int screenH);
		void worldResized(const unsigned int worldW, const unsigned int worldH);

		const Rectangle* getDisplay() const;
		~CameraSystem();

	private:
		unsigned int m_worldW, m_worldH;
		PositionComponent* m_pos;
		Rectangle m_cameraRect;
	};
}
