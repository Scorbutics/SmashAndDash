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

		const Rectangle* getDisplay() const;
		~CameraSystem();

	private:
		PositionComponent* m_pos;
		Rectangle m_cameraRect;
	};
}
