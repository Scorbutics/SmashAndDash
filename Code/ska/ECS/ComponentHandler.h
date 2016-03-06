#pragma once
#include <vector>
#include "ECSDefines.h"

namespace ska {

	
	template <typename T>
	class ComponentHandler
	{
		
	public:
		ComponentHandler() {
			m_components.resize(SKA_ECS_MAX_ENTITIES);
			m_mask = m_componentTypeCounter++;
		}

		unsigned int add(EntityId entityId, const T& comp) {
			m_allEntities.push_back(entityId);
			m_components[entityId] = std::move(std::unique_ptr<T>(new T(comp)));
			m_components[entityId]->setMask(m_mask);
			return m_mask;
		}
		
		std::unique_ptr<T>& getComponent(const EntityId id) {
			return m_components[id];
		}

		const unsigned int getMask() const {
			return m_mask;
		}

		EntityIdContainer& getEntities() {
			return m_allEntities;
		}

		~ComponentHandler() {
		}


	private:
		std::vector<std::unique_ptr<T>> m_components;
		EntityIdContainer m_allEntities;
		unsigned int m_mask;
		static unsigned int m_componentTypeCounter;

	};

	template <typename T>
	unsigned int ComponentHandler<T>::m_componentTypeCounter = 0;

	
}