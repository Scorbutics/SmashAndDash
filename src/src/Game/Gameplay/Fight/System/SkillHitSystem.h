#pragma once
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "../SkillComponent.h"
#include "../BattleStage.h"
#include "../BattleComponent.h"
#include "../StatisticsChangeEvent.h"
#include "../../PokemonGameEventDispatcher.h"

class BattleRules;

// A skill touching the Pokemon it was used against removes hit points (damage given by the battle rules), once :
// the skill disappears. The hit is felt : everything stops a moment (hit stop), the Pokemon hit flashes, is stunned
// and pushed back, the camera shakes, the damage appears (BattleStage).
class SkillHitSystem : public ska::System<ska::RequiredComponent<SkillComponent, ska::PositionComponent, ska::HitboxComponent>, ska::PossibleComponent<>> {
public:
	SkillHitSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, BattleRules& rules, BattleStage& stage);
	~SkillHitSystem() override = default;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	bool touches(ska::EntityId skill, ska::EntityId target);
	void hit(ska::EntityId skillEntity, SkillComponent& skill, ska::EntityId target);

	ska::EntityManager& m_entities;
	PokemonGameEventDispatcher& m_ged;
	BattleRules& m_rules;
	BattleStage& m_stage;
};
