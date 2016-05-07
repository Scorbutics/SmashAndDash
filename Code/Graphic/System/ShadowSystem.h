#pragma once
#include <unordered_set>
#include "../../ska/Physic/PositionComponent.h"
#include "../../ska/ECS/System.h"
#include "../../ska/Graphic/HasShadowComponent.h"
#include "../../ska/Physic/HitboxComponent.h"
#include "AbstractGraphicSystem.h"

class ShadowSystem : public AbstractGraphicSystem, public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, ska::HitboxComponent, ska::HasShadowComponent>
{
public:
	ShadowSystem(ska::CameraSystem* camera, ska::EntityManager& entityManager);
	virtual ~ShadowSystem();
	virtual void update() override;
protected:
	virtual void refresh() override;
	
private:
	ska::AnimatedTexture m_shadow;
};


