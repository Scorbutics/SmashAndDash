#pragma once
#include <vector>
#include <string>
#include "ECSDefines.h"
#include "StaticCounterGlobal.h"
#include "ComponentSerializer.h"

namespace ska {
	template <typename T>
	class ComponentHandler : public StaticCounterGlobal, public ComponentSerializer
	{
		
	public:
		ComponentHandler() {
			m_components.resize(SKA_ECS_MAX_ENTITIES);
			m_mask = m_componentTypeCounter++;
		}

		unsigned int add(EntityId entityId, const T& comp) {
			m_components[entityId] = comp;
			return m_mask;
		}
		
		unsigned int remove(EntityId entityId) {
			/* We can add operations here depending of the ComponentHandler implementation */
			return m_mask;
		}

		T& getComponent(const EntityId id) {
			return m_components[id];
		}

		virtual const std::string getComponentField(const EntityId id, const std::string& field) override {
			return m_components[id].serialize(m_components[id], field, getClassName());
		}

		const unsigned int getMask() const {
			return m_mask;
		}

		~ComponentHandler() {
		}

		static const std::string& getClassName() {
			static const std::string fullClassName = std::string(typeid(T).name());
			static const size_t startPos = fullClassName.find_last_of(':');
			static const std::string& name = fullClassName.substr((startPos == std::string::npos ? -1 : startPos) + 1);
			return name;
		}

	private:
		std::vector<T> m_components;
		unsigned int m_mask;
	};
	

}