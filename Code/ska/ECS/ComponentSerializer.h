#pragma once
#include <string>
#include "ECSDefines.h"

namespace ska {
	class ComponentSerializer {
	public:
		ComponentSerializer() = default;
		virtual ~ComponentSerializer() = default;

		virtual const std::string getComponentField(const EntityId id, const std::string& field) = 0;
	};
}