#pragma once
#include <unordered_set>
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/System.h"
#include "../HasShadowComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "AbstractGraphicSystem.h"

namespace ska {
    class ShadowSystem :
        public AbstractGraphicSystem,
        public System<std::unordered_set<EntityId>, PositionComponent, HitboxComponent, HasShadowComponent> {
    public:
        ShadowSystem(CameraSystem* camera, EntityManager& entityManager);
        ShadowSystem& operator=(const ShadowSystem&) = delete;
        virtual ~ShadowSystem();
        virtual void update() override;
    protected:
        virtual void refresh() override;

    private:
        AnimatedTexture m_shadow;
    };
}
