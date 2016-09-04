#pragma once
#include <bitset>
#include <unordered_set>
#include <array>
#include <unordered_map>
#include <string>
#include "ECSDefines.h"
#include "ComponentHandler.h"
#include "../Exceptions/IllegalStateException.h"
#include "../Exceptions/IllegalArgumentException.h"
#include "../Utils/Observable.h"
#include "ComponentSerializer.h"
#include "../Utils/StringUtils.h"

namespace ska {

	typedef std::bitset<SKA_ECS_MAX_COMPONENTS> EntityComponentsMask;
	typedef std::pair<EntityComponentsMask*, EntityId> EntityData;

	enum EntityEventType {
		COMPONENT_ADD,
		COMPONENT_REMOVE
	};

	class EntityManager : public Observable<const EntityEventType, const EntityComponentsMask&, EntityId> {
	public:
		EntityManager() { }
		EntityId createEntity() {
			EntityId newId;
			if (((int)(m_entities.size() - m_deletedEntities.size())) >= SKA_ECS_MAX_ENTITIES) {
				throw IllegalStateException("Too many entities are currently in use. Unable to create a new one. "
					"Increase SKA_ECS_MAX_ENTITIES at compile time to avoid the problem.");
			}

			if (m_deletedEntities.empty()) {
				newId = (EntityId)m_entities.size();
				m_entities.insert(newId);
			} else {
				newId = m_deletedEntities[m_deletedEntities.size() - 1];
				m_deletedEntities.pop_back();
				m_entities.insert(newId);
			}
			
			return newId;
		}
		
		void removeEntity(EntityId entity) {
			if (m_entities.find(entity) == m_entities.end() || m_entities.count(entity) <= 0) {
				std::string startMessage = ("Unable to delete entity #" + ska::StringUtils::intToStr(entity));
				throw IllegalArgumentException(startMessage + " : this entity doesn't exist or is already deleted");
			}

			m_entities.erase(entity);

			m_deletedEntities.push_back(entity);

			/* Reset all components */
			m_componentMask[entity] &= 0;

			notifyObservers(EntityEventType::COMPONENT_REMOVE, m_componentMask[entity], entity);
		}

		void removeEntities(const std::unordered_set<EntityId>& exceptions) {
			std::unordered_set<EntityId> entities = m_entities;
			for (EntityId entity : entities) {
				if (exceptions.find(entity) == exceptions.end()) {
					removeEntity(entity);
				}
			}
		}

		void refreshEntity(EntityId entity) {
			notifyObservers(EntityEventType::COMPONENT_ADD, m_componentMask[entity], entity);
		}

		void refreshEntities() {
			for (const auto& entity : m_entities) {
				notifyObservers(EntityEventType::COMPONENT_ADD, m_componentMask[entity], entity);
			}
		}

		template <class T>
		void addComponent(EntityId entity, const T& component) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			commonAddComponent(entity, components.add(entity, component));
		}

		template <class T>
		T& getComponent(EntityId entityId) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			T& result = components.getComponent(entityId);
			return result;
		}

		const std::string serializeComponent(const EntityId entityId, const std::string& component, const std::string& field) {
			if (m_nameMappedComponent.find(component) != m_nameMappedComponent.end()) {
				return m_nameMappedComponent[component]->getComponentField(entityId, field);
			}
			return "";
		}

		void removeComponent(const EntityId entity, const std::string& component) {
			if (m_nameMappedComponent.find(component) != m_nameMappedComponent.end()) {
				ComponentSerializer * components = m_nameMappedComponent[component];
				commonRemoveComponent(entity, *components);
			}
		}

		void addComponent(const EntityId entity, const std::string& component) {
			if (m_nameMappedComponent.find(component) != m_nameMappedComponent.end()) {
				ComponentSerializer * components = m_nameMappedComponent[component];
				commonAddComponent(entity, components->addEmpty(entity));
			}
		}

		template <class T>
		bool hasComponent(EntityId entityId) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			return m_componentMask[entityId][components.getMask()];	
		}

		template <class T>
		void removeComponent(EntityId entity) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			commonRemoveComponent(entity, components);
		}

		template <class T>
		unsigned int getMask() {
			ComponentHandler<T>& components = this->template getComponents<T>();
			return components.getMask();
		}

		virtual ~EntityManager() { }
	
	private:
		std::array<EntityComponentsMask, SKA_ECS_MAX_ENTITIES> m_componentMask;
		std::unordered_set<EntityId> m_entities;
		EntityIdContainer m_deletedEntities;

		std::unordered_map<std::string, ComponentSerializer*> m_nameMappedComponent;

		template <class T>
		ComponentHandler<T>& getComponents() {
			static ComponentHandler<T> m_components;
			static bool initialized = false;
			/* Correction performances issues du Profiling Very Sleepy : temps passé dans le hachage pour la table de noms de composants trop élevé */
			if (!initialized) {
				initialized = true;
				m_nameMappedComponent.emplace(m_components.getClassName(), &m_components);
			}
			return m_components;
		}

		void commonRemoveComponent(EntityId entity, ComponentSerializer& components) {
			unsigned int removedComponentMask = components.remove(entity);
			m_componentMask[entity][removedComponentMask] = false;

			notifyObservers(EntityEventType::COMPONENT_REMOVE, m_componentMask[entity], entity);
		}

		void commonAddComponent(EntityId entity, const unsigned int componentMask) {
			m_componentMask[entity][componentMask] = true;
			notifyObservers(EntityEventType::COMPONENT_ADD, m_componentMask[entity], entity);
		}
		
	};

}
