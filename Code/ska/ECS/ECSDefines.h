#pragma once
#include <vector>
#include <unordered_map>

#define SKA_ECS_MAX_ENTITIES 2000
#define SKA_ECS_MAX_COMPONENTS 50

namespace ska {
	typedef unsigned int EntityId;
	typedef unsigned int ComponentId;
	
	typedef std::vector<EntityId> EntityIdContainer;
}
