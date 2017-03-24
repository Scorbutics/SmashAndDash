#pragma once
#include <string>
#include "ECSDefines.h"

namespace ska {
	class ComponentSerializer {
	public:
		ComponentSerializer() = default;
		virtual ~ComponentSerializer() = default;

		virtual std::string getComponentField(const EntityId id, const std::string& field) = 0;
		
		virtual unsigned int remove(EntityId entityId) = 0;
		virtual unsigned int addEmpty(EntityId entityId) = 0;
	};
}
