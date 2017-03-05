#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../InputComponent.h"
#include "../../Physic/ForceComponent.h"
#include "../../Physic/PositionComponent.h"
#include "../../Utils/Observer.h"
#include "../InputContextManager.h"

namespace ska {
	class InputSystem : public System<std::unordered_set<EntityId>, PositionComponent, InputComponent, ForceComponent> {
	public:
		InputSystem(const InputContextManager& icm, EntityManager& entityManager);
		InputSystem& operator=(const InputSystem&) = delete;
		virtual ~InputSystem();
	protected:
		virtual void refresh() override;
	private:
		const InputContextManager& m_icm;
	};
}
