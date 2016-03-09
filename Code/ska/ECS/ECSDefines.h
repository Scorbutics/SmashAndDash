#pragma once
#include <vector>
#include <unordered_map>

#define SKA_ECS_MAX_ENTITIES 1000
#define SKA_ECS_MAX_COMPONENTS 200

namespace ska {
	typedef unsigned int EntityId;
	typedef unsigned int ComponentId;
	
	typedef std::vector<EntityId> EntityIdContainer;
}
