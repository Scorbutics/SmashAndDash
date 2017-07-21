#pragma once
#include <unordered_set>
#include "../GraphicComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/System.h"
#include "AbstractGraphicSystem.h"
#include <Data/Events/EventDispatcher.h>
#include <Data/Events/GameEventDispatcher.h>
#include "ECS/Basics/Graphic/DialogComponent.h"

namespace ska {
    class GraphicSystem : public AbstractGraphicSystem, 
		public System<std::unordered_set<EntityId>, RequiredComponent<GraphicComponent, PositionComponent>, PossibleComponent<DialogComponent>> {
    public:
		GraphicSystem(EntityManager& entityManager, ska::GameEventDispatcher& ged, CameraSystem* camera);
        GraphicSystem& operator=(const GraphicSystem&) = delete;
        int getTopLayerPriority() const;
        virtual void update(unsigned int ellapsedTime) override;
        virtual ~GraphicSystem();
    protected:
        virtual void refresh(unsigned int ellapsedTime) override;
    private:
		ska::GameEventDispatcher& m_ged;
        int m_topLayerPriority;
    };
}
