#pragma once
#include <unordered_set>
#include "../../Component.h"

namespace ska {
	class CollidableComponent : public Component {
	public:
		CollidableComponent();
		~CollidableComponent() = default;
		std::unordered_set<int> authorizedBlockIds;
	};
}
