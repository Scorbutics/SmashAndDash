#pragma once
#include "ECSDefines.h"
#include "EntityManager.h"
#include "../Exceptions/IllegalStateException.h"

namespace ska {
    template <class ... ComponentType>
    class SystemMaskGenerator {
    public:
        SystemMaskGenerator(EntityManager& entityManager) : m_entityManager(entityManager) {
        }
        ~SystemMaskGenerator() = default;

        void generate(EntityComponentsMask& ecm) {
            SKA_LOG_MESSAGE("Initializing system with components :");
            int _[] = { 0, (buildSystemMask<ComponentType>(ecm) , 0)... };
			(void)_;
			SKA_LOG_MESSAGE("End system initialization\n\n");;
        }

    private:
        template <class T>
		void buildSystemMask(EntityComponentsMask& systemComponentMask) {
			/* Retrieve all the components masks using the variadic template with ComponentType
			   We "iterate" through each ComponentType with the bracket initializer trick and
			   we add each component mask to the system component mask with a binary OR. */
			unsigned int mask = m_entityManager.template getMask<T>();
			if (mask >= systemComponentMask.size()) {
				throw IllegalStateException("Too many components are used in the game. Unable to continue.");
			}
			SKA_LOG_MESSAGE("\t - ", m_entityManager.template getComponentName<T>(), " with mask ", mask);

			systemComponentMask[mask] = true;
		}

		EntityManager& m_entityManager;
    };
}
