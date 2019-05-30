#pragma once
#include <unordered_set>
#include "ECS/System.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "../SkillComponent.h"
#include "AI/System/IADefinedMovementSystem.h"

class SkillRefreshSystem : public ska::System< ska::RequiredComponent<ska::PositionComponent, ska::MovementComponent, SkillComponent, ska::ForceComponent>, ska::PossibleComponent<>> {
public:
	explicit SkillRefreshSystem(ska::EntityManager& em);
	SkillRefreshSystem& operator=(const SkillRefreshSystem&) = delete;
	virtual ~SkillRefreshSystem();

protected:
	virtual void refresh(unsigned int ellapsedTime) override;

private:

};
