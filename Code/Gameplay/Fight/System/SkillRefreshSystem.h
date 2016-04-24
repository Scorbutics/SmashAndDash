#pragma once
#include <unordered_set>
#include "../../../ska/ECS/System.h"
#include "../../../ska/Physic/PositionComponent.h"
#include "../../../ska/Physic/MovementComponent.h"
#include "../SkillComponent.h"



class SkillRefreshSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, ska::MovementComponent, SkillComponent> {
public:
	SkillRefreshSystem(ska::EntityManager& em);
	void refresh() override;
	virtual ~SkillRefreshSystem();

private:

};