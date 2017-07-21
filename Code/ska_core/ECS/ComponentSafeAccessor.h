#pragma once
#include "ECSDefines.h"
#include "EntityManager.h"
#include "../Utils/ContainsTypeTuple.h"

namespace ska {
    template <class ... ComponentType>
    class ComponentSafeAccessor {
    public:
        ComponentSafeAccessor(EntityManager& entityManager) :
            m_entityManager(entityManager) {
        }
        ~ComponentSafeAccessor() = default;

        template <class T>
        T& getComponent(EntityId entityId) const {
            using componentTypeFound = meta::contains<T, ComponentType...>;
            static_assert(componentTypeFound::value, "Unable to use this component : it doesn't belong to the current system");
            return m_entityManager.getComponent<T>(entityId);
		}
    private:
        EntityManager& m_entityManager;
    };
}
