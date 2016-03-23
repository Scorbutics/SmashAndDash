#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../InputComponent.h"
#include "../../Physic/ForceComponent.h"
#include "../../Physic/PositionComponent.h"
#include "../../Utils/Observer.h"
#include "../InputContextManager.h"

namespace ska {
	class InputSystem : public ska::System<std::unordered_set<ska::EntityId>, PositionComponent, InputComponent, ForceComponent> {
	public:
		InputSystem(const InputContextManager& icm, ska::EntityManager& entityManager);
		virtual ~InputSystem();
		virtual void refresh() override;
	private:
		const InputContextManager& m_icm;
	};
}
