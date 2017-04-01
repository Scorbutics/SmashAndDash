#pragma once
#include <unordered_set>
#include "../GraphicComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/System.h"
#include "AbstractGraphicSystem.h"

namespace ska {
    class GraphicSystem : public AbstractGraphicSystem, public System<std::unordered_set<EntityId>, GraphicComponent, PositionComponent> {
    public:
		GraphicSystem(EntityManager& entityManager, CameraSystem* camera);
        GraphicSystem& operator=(const GraphicSystem&) = delete;
        int getTopLayerPriority() const;
        virtual void update() override;
        virtual ~GraphicSystem();
    protected:
        virtual void refresh() override;
    private:
        int m_topLayerPriority;
    };
}
