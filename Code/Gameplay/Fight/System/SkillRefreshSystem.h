#pragma once
#include <unordered_set>
#include "../../../ska/ECS/System.h"
#include "../../../ska/Physic/PositionComponent.h"
#include "../../../ska/Physic/MovementComponent.h"
#include "../../../ska/Physic/ForceComponent.h"
#include "../SkillComponent.h"



class SkillRefreshSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, ska::MovementComponent, SkillComponent, ska::ForceComponent> {
public:
	SkillRefreshSystem(ska::EntityManager& em);
	virtual ~SkillRefreshSystem();
protected:
	virtual void refresh() override;

private:

};