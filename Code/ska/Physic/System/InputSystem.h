#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../InputComponent.h"
#include "../ForceComponent.h"
#include "../../Utils/Observer.h"

namespace ska {
	class InputSystem : public ska::System<std::unordered_set<ska::EntityId>, InputComponent>, public ska::Observer<const int> {
	public:
		InputSystem(ska::EntityManager& entityManager);
		virtual void update(ska::Observable<const int>* obs, const ska::EventArg& e, const int& t) override;
		virtual ~InputSystem();
		virtual void refresh() override;

	};
}
