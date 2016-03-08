#pragma once
#include <vector>
#include "ECSDefines.h"
#include "StaticCounterGlobal.h"

namespace ska {
	template <typename T>
	class ComponentHandler : public StaticCounterGlobal
	{
		
	public:
		ComponentHandler() {
			m_components.resize(SKA_ECS_MAX_ENTITIES);
			m_mask = m_componentTypeCounter++;
		}

		unsigned int add(EntityId entityId, const T& comp) {
			m_components[entityId] = comp;
			m_components[entityId].setMask(m_mask);
			return m_mask;
		}
		
		unsigned int remove(EntityId entityId) {
			m_components[entityId].setMask(0);
			return m_mask;
		}

		T& getComponent(const EntityId id) {
			return m_components[id];
		}

		const unsigned int getMask() const {
			return m_mask;
		}

		~ComponentHandler() {
		}

	private:
		std::vector<T> m_components;
		unsigned int m_mask;

	};

	
}