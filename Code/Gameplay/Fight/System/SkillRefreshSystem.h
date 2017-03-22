#pragma once
#include <unordered_set>
#include "../../../ska/ECS/System.h"
#include "../../../ska/Physic/PositionComponent.h"
#include "../../../ska/Physic/MovementComponent.h"
#include "../../../ska/Physic/ForceComponent.h"
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
