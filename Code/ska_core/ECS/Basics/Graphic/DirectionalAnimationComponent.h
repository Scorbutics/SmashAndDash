#pragma once
#include "../../Component.h"
#include "../../../Utils/StringUtils.h"
namespace ska {
	namespace DirectionalAnimationType {
		enum Enum {
			MOVEMENT,
			MOVEMENT_THEN_FOLLOWING
		};
	}

	class DirectionalAnimationComponent : public Component {
	public:
		DirectionalAnimationComponent();

		int direction;
		DirectionalAnimationType::Enum type;
		EntityId looked;

	protected:
		static std::string serializeDirection(const Component& component) {
			return StringUtils::intToStr(static_cast<const DirectionalAnimationComponent&>(component).direction);
		}

	};
}
