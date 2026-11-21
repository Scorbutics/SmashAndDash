#pragma once
#include <unordered_set>
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include "../SkillComponent.h"
#include "Graphic/GraphicComponent.h"
#include "../BattleStage.h"
#include "AI/System/IADefinedMovementSystem.h"

class SkillRefreshSystem : public ska::System< ska::RequiredComponent<ska::PositionComponent, ska::MovementComponent, SkillComponent, ska::ForceComponent>, ska::PossibleComponent<ska::GraphicComponent>> {
public:
	// The skills move with the time of the battle (stopped during a hit stop)
	SkillRefreshSystem(ska::EntityManager& em, BattleStage& stage);
	SkillRefreshSystem& operator=(const SkillRefreshSystem&) = delete;
	virtual ~SkillRefreshSystem();

protected:
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	BattleStage& m_stage;

};
