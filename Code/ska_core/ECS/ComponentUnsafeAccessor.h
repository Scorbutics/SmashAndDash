#pragma once
#include "ECSDefines.h"
#include "EntityManager.h"
#include "../Utils/ContainsTypeTuple.h"

namespace ska {
    template <class ... ComponentType>
    class ComponentUnsafeAccessor {
    public:
        ComponentUnsafeAccessor(EntityManager& entityManager) :
            m_entityManager(entityManager) {
        }
        ~ComponentUnsafeAccessor() = default;

        template <class T>
        T* get(EntityId entityId) const {
            using componentTypeFound = meta::contains<T, ComponentType...>;
            static_assert(componentTypeFound::value, "Unable to use this component : it doesn't belong to the current system");
			if(m_entityManager.hasComponent<T>(entityId)) {
				return &m_entityManager.getComponent<T>(entityId);
			}
			return nullptr;
		}
    private:
        EntityManager& m_entityManager;
    };
}
