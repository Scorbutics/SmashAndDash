#include "Component.h"

std::unordered_map<std::string, ska::field_serializer_method> ska::Component::m_fieldSerializer;

const std::string ska::Component::serialize(const Component& component, const std::string& field, const std::string& className) {
	const std::string fullFieldName = className + "." + field;
	if (m_fieldSerializer.find(fullFieldName) != m_fieldSerializer.end()) {
		return (*m_fieldSerializer[fullFieldName])(component);
	}
	return "";
}

void ska::Component::addFieldSerializer(field_serializer_method serializer, const std::string& fieldName, const std::string& className) {
	const std::string fullFieldName = className + "." + fieldName;
	m_fieldSerializer.emplace(fullFieldName, serializer);
}
