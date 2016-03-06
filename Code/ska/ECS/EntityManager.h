#pragma once
#include <bitset>
#include <vector>
#include "ECSDefines.h"
#include "Component/GraphicComponent.h"
#include "ComponentHandler.h"

namespace ska {


	class EntityManager
	{
	public:
		EntityManager() { m_componentMask.resize(SKA_ECS_MAX_ENTITIES); }
		EntityId createEntity() const {

		}
		
		void removeEntity(EntityId) {

		}

		template <class T>
		void addComponent(EntityId entity, const T& component) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			unsigned int addedComponentMask = components.add(entity, component);
			m_componentMask[entity][addedComponentMask] = true;
		}

		template <class T>
		EntityIdContainer& getEntitiesWithComponent() {
			ComponentHandler<T>& components = this->template getComponents<T>();
			return components.getEntities();
		}

		template <class T>
		std::unique_ptr<T>& getComponent(EntityId entityId) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			return components.getComponent(entityId);
		}


		virtual ~EntityManager() {}
	
	private:
		std::vector<std::bitset<SKA_ECS_MAX_COMPONENTS>> m_componentMask;

		template <class T>
		ComponentHandler<T>& getComponents() {
			static ComponentHandler<T> m_components;
			return m_components;
		}
		
	};
}
