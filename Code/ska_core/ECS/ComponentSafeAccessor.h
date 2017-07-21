#pragma once
#include "ECSDefines.h"
#include "EntityManager.h"
#include "../Utils/ContainsTypeTuple.h"

namespace ska {
    template <class ... ComponentType>
    class ComponentSafeAccessor {
    public:
	    explicit ComponentSafeAccessor(EntityManager& entityManager) :
            m_entityManager(entityManager) {
        }
        ~ComponentSafeAccessor() = default;

		template <class T>
		static void checkType() {
			using componentTypeFound = meta::contains<T, ComponentType...>;
			static_assert(componentTypeFound::value, "Unable to use this component : it doesn't belong to the current system");
		}

        template <class T>
        T& get(EntityId entityId) const {
			checkType<T>();
            return m_entityManager.getComponent<T>(entityId);
		}

		template <class T>
		void add(EntityId entity, const T& component) {
			m_entityManager.addComponent<T>(entity, component);
        }

		void add(const EntityId entity, const std::string& componentName) const {
			m_entityManager.addComponent(entity, componentName);
		}

		template <class T>
		void remove(const EntityId entity) const {
			m_entityManager.removeComponent<T>(entity);
        }

		void remove(const EntityId entity, const std::string& componentName) const {
			m_entityManager.removeComponent(entity, componentName);
		}

		template <class T>
		std::string serialize(const EntityId entity, const T& component, const std::string& field) const {
			return m_entityManager.serializeComponent(entity, component, field);
		}

    private:
        EntityManager& m_entityManager;
    };
}
