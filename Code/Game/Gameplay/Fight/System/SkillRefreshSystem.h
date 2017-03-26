#pragma once
#include <unordered_set>
#include "ECS/System.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "../SkillComponent.h"

class SkillRefreshSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, ska::MovementComponent, SkillComponent, ska::ForceComponent> {
public:
	explicit SkillRefreshSystem(ska::EntityManager& em);
	SkillRefreshSystem& operator=(const SkillRefreshSystem&) = delete;
	virtual ~SkillRefreshSystem();

protected:
	virtual void refresh() override;

private:

};
