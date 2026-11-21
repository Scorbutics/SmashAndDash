#pragma once
#include <array>
#include <unordered_set>
#include "Base/Patterns/SubObserver.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "../../PokemonGameEventDispatcher.h"
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "../BattleComponent.h"
#include "../BattleStage.h"
#include "../SkillsHolderComponent.h"
#include "../SkillComponent.h"
#include "Graphic/GraphicComponent.h"

namespace ska {
	class InputContextManager;
	class IniReader;
}

class CustomEntityManager;
class BattleRules;

// Real time battle : the Pokemon of the player uses its skills with the keys 1 to 4, the wild Pokemon uses its skills
// and moves as its AI (script, see BattleRules) decides. A skill has a short wind-up, then it goes towards the opponent.
// It can be used again after its cooldown. The time of the battle stops during the hit stops (BattleStage).
class BattleSystem :
	public ska::System< ska::RequiredComponent<ska::PositionComponent, ska::HitboxComponent, ska::AnimationComponent, BattleComponent, SkillsHolderComponent, ska::GraphicComponent>, ska::PossibleComponent<SkillComponent>>,
	public ska::SubObserver<ska::InputKeyEvent> {
public:
	BattleSystem(CustomEntityManager& em, PokemonGameEventDispatcher& ged, BattleRules& rules, BattleStage& stage, unsigned int blockSize);
	BattleSystem(const BattleSystem&) = delete;

	BattleSystem& operator=(const BattleSystem&) = delete;

	// Starts a skill (its wind-up) when it is ready and the fighter can act : returns false otherwise
	bool useSkill(unsigned int index, ska::EntityId from);
	bool skillReady(unsigned int index, ska::EntityId from) const;
	// Part of the cooldown of a skill left, from 1 (just used) to 0 (ready)
	float cooldown(unsigned int index, ska::EntityId from) const;
	~BattleSystem() override = default;

	static constexpr unsigned int DecisionDelay = 300;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	bool onKeyInputEvent(ska::InputKeyEvent& event);
	bool fighting(ska::EntityId fighter) const;
	bool canAct(ska::EntityId fighter) const;
	void createSkill(unsigned int index, ska::EntityId from);
	void decide(FighterState& fighter);
	ska::Point<float> center(ska::EntityId entity) const;
	std::array<unsigned long, 4>& lastUses(ska::EntityId fighter);
	const std::array<unsigned long, 4>& lastUses(ska::EntityId fighter) const;

	CustomEntityManager& m_customEM;
	BattleRules& m_rules;
	BattleStage& m_stage;
	const unsigned int m_blockSize;
	// Time of the battle (milliseconds), time of the last use of each skill (0 : never used)
	unsigned long m_time = 0;
	// Each AI decides at its own time (not both at once)
	std::array<unsigned long, 2> m_nextDecisions { DecisionDelay, DecisionDelay + DecisionDelay / 2 };
	std::array<unsigned long, 4> m_pokemonUses {};
	std::array<unsigned long, 4> m_opponentUses {};
};
