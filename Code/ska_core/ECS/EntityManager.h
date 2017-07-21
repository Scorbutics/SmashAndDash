#pragma once
#include <bitset>
#include <unordered_set>
#include <array>
#include <unordered_map>
#include "ECSDefines.h"
#include "ComponentHandler.h"
#include "../Utils/Observable.h"
#include "ComponentSerializer.h"

namespace ska {

	typedef std::bitset<SKA_ECS_MAX_COMPONENTS> EntityComponentsMask;
	typedef std::pair<EntityComponentsMask*, EntityId> EntityData;

	enum EntityEventType {
		COMPONENT_ADD,
		COMPONENT_REMOVE,
		COMPONENT_ALTER
	};

	class EntityManager :
	    public Observable<const EntityEventType, const EntityComponentsMask&, EntityId> {
	public:
		EntityManager() : m_componentMask() { }

		EntityId createEntity();
		void removeEntity(EntityId entity);
		void removeEntities(const std::unordered_set<EntityId>& exceptions);
		void refreshEntity(EntityId entity);
		void refreshEntities();
        void refresh();

		const std::string serializeComponent(const EntityId entityId, const std::string& component, const std::string& field) const;
		void removeComponent(const EntityId entity, const std::string& component);
		void addComponent(const EntityId entity, const std::string& component);

        template <class T>
		void addComponent(EntityId entity, const T& component) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			commonAddComponent(entity, components.add(entity, component));
		}

		template <class T>
		T& getComponent(EntityId entityId) const {
			ComponentHandler<T>& components = this->template getComponents<T>();
			T& result = components.getComponent(entityId);
			return result;
		}

		template <class T>
		bool hasComponent(EntityId entityId) const {
			ComponentHandler<T>& components = this->template getComponents<T>();
			return m_componentMask[entityId][components.getMask()];
		}

		template <class T>
		void removeComponent(EntityId entity) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			commonRemoveComponent(entity, components);
		}

		template <class T>
		unsigned int getMask() const {
			ComponentHandler<T>& components = this->template getComponents<T>();
			return components.getMask();
		}

        template <class T>
		std::string getComponentName() const {
			ComponentHandler<T>& components = this->template getComponents<T>();
			return components.getClassName();
		}

		virtual ~EntityManager() = default;

	private:
		std::array<EntityComponentsMask, SKA_ECS_MAX_ENTITIES> m_componentMask;
		std::unordered_set<EntityId> m_entities;
		std::unordered_set<EntityId> m_alteredEntities;
		EntityIdContainer m_deletedEntities;

		static std::unordered_map<std::string, ComponentSerializer*> NAME_MAPPED_COMPONENT;

		template <class T>
		static ComponentHandler<T>& getComponents() {
			static ComponentHandler<T> components;
			static auto initialized = false;
			/* Correction performances issues du Profiling Very Sleepy : temps passé dans le hachage pour la table de noms de composants trop élevé */
			if (!initialized) {
				initialized = true;
				NAME_MAPPED_COMPONENT.emplace(components.getClassName(), &components);
			}
			return components;
		}

		void commonRemoveComponent(EntityId entity, ComponentSerializer& components);
		void commonAddComponent(EntityId entity, const unsigned int componentMask);

	};

}
