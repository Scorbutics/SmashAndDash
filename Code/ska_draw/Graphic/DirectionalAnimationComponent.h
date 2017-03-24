#pragma once
#include "../ECS/Component.h"
#include "../Utils/StringUtils.h"
namespace ska {
	namespace DirectionalAnimationType {
		enum Enum {
			MOVEMENT,
			MOVEMENT_THEN_FOLLOWING
		};
	}

	class DirectionalAnimationComponent : public Component {
	public:
		DirectionalAnimationComponent() : type(DirectionalAnimationType::MOVEMENT), looked(0) {
			static auto initialized = false;
			if (!initialized) {
				initialized = true;
				const auto className = ComponentHandler<std::remove_reference<decltype(*this)>::type>::getClassName();
				addFieldSerializer(serializeDirection, "direction", className);
			}
			direction = 0;
		}

		int direction;
		DirectionalAnimationType::Enum type;
		EntityId looked;

	protected:
		static std::string serializeDirection(const Component& component) {
			return StringUtils::intToStr(static_cast<const DirectionalAnimationComponent&>(component).direction);
		}

	};
}
