#pragma once
#include <unordered_map>
#include "ComponentHandler.h"

namespace ska {
	
	class Component;
	typedef std::string(*field_serializer_method)(const Component& component);
	
	class Component {
	public:
		Component() = default;
		
		const std::string serialize(const Component& component, const std::string& field, const std::string& className);
		
		virtual ~Component() = default;
	
	protected:
		void addFieldSerializer(field_serializer_method serializer, const std::string& fieldName, const std::string& className);

	private:
		static std::unordered_map<std::string, field_serializer_method> m_fieldSerializer;
	};
	
}
