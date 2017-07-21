#pragma once
#include <unordered_set>
#include "SystemMaskGenerator.h"
#include "ComponentSafeAccessor.h"
#include "ISystem.h"
#include "../Utils/Observer.h"
#include "../Utils/Refreshable.h"
#include "../Logging/Logger.h"
#include "ComponentUnsafeAccessor.h"

namespace ska {
    template <class ... ComponentType>
    struct RequiredComponent;

    template <class ... ComponentType>
    struct PossibleComponent;

	template <class Storage, class RComponentType, class PComponentType>
	class System;

    template <class Storage, class ... RComponentType, class ... PComponentType>
    class System <Storage, RequiredComponent<RComponentType...>, PossibleComponent<PComponentType...>> :
        public Observer<const EntityEventType, const EntityComponentsMask&, EntityId>,
        virtual public ISystem,
        virtual protected Refreshable {

    protected:
        using SystemType = System <Storage, RequiredComponent<RComponentType...>, PossibleComponent<PComponentType...>>;
        using MaskGenerator = SystemMaskGenerator<RComponentType...>;

    public :
        explicit System(EntityManager& entityManager) :
            Observer(std::bind(&System::onComponentModified, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
            m_entityManager(entityManager),
            m_componentAccessor(entityManager),
    		m_componentPossibleAccessor(entityManager) {
            m_entityManager.addObserver(*this);

            MaskGenerator smg(m_entityManager);
            smg.generate(m_systemComponentMask);
        }

        void operator=(const SystemType& sys) = delete;

        void update(unsigned int ellapsedTime) override {
            refresh(ellapsedTime);
            if (!m_toDelete.empty()) {
                for (auto entity : m_toDelete) {
                    m_entityManager.removeEntity(entity);
                }
                m_toDelete.clear();
            }
        }

        void scheduleDeferredRemove(EntityId e) {
            m_toDelete.emplace(e);
        }

        virtual ~System(){ m_entityManager.removeObserver(*this); }

    private:
        EntityManager& m_entityManager;
        std::unordered_set<EntityId> m_toDelete;
        EntityComponentsMask m_systemComponentMask;
        Storage m_processed;

        bool onComponentModified(const EntityEventType& e, const EntityComponentsMask& mask, EntityId entityId) {

            /* An entity belongs to the system ONLY IF it has ALL the requiered components of the system */
            auto resultMask = mask & m_systemComponentMask;

            switch (e) {
            case COMPONENT_ADD:
                if (resultMask == m_systemComponentMask && m_processed.count(entityId) == 0) {
                    m_processed.insert(entityId);
                }
                break;

            case COMPONENT_REMOVE:
                if (resultMask != m_systemComponentMask && m_processed.count(entityId) > 0) {
                    m_processed.erase(entityId);
                }
                break;

            case COMPONENT_ALTER:
                if(resultMask == m_systemComponentMask) {
                    m_processed.insert(entityId);
                } else {
                    m_processed.erase(entityId);
                }
                break;

            default:
                break;
            }
            return true;
        }

    protected:
        using ComponentAccessor = ComponentSafeAccessor<RComponentType...>;
		using ComponentPossibleAccessor = ComponentUnsafeAccessor<PComponentType...>;
        ComponentAccessor m_componentAccessor;
		ComponentPossibleAccessor m_componentPossibleAccessor;

        const Storage& getEntities() {
            return m_processed;
        }

		EntityId createEntity() const{
			return m_entityManager.createEntity();
		}

		void removeEntity(EntityId entity) const {
			m_entityManager.removeEntity(entity);
        }

		void refreshEntity(EntityId entity) const {
			m_entityManager.refreshEntity(entity);
        }
    };

}
