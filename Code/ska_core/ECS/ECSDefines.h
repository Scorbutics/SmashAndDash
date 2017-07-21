#pragma once
#include <vector>
#include <unordered_map>

#define SKA_ECS_MAX_ENTITIES 1000
#define SKA_ECS_MAX_COMPONENTS 50

namespace ska {
	using EntityId = unsigned int ;
	using ComponentId = unsigned int ;
	
	using EntityIdContainer = std::vector<EntityId>;
}
