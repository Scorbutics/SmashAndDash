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
        public System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, HitboxComponent, HasShadowComponent>, PossibleComponent<>> {
    public:
		ShadowSystem(EntityManager& entityManager, CameraSystem* camera);
        ShadowSystem& operator=(const ShadowSystem&) = delete;
        virtual ~ShadowSystem();
        virtual void update(unsigned int ellapsedTime) override;
    protected:
        virtual void refresh(unsigned int ellapsedTime) override;

    private:
        AnimatedTexture m_shadow;
    };
}
