#pragma once
#include <vector>
#include <unordered_map>

#define SKA_ECS_MAX_ENTITIES 1000

namespace ska {
	typedef int EntityId;
	
	template<class T>
	using ComponentContainer = std::unordered_map<EntityId, T>;

	typedef std::vector<EntityId> EntityContainer;
}