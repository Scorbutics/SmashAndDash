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
#include "../FighterComponent.h"
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
// and moves as its AI (script, see BattleRules) decides. An attack has a startup (the Pokemon prepares it), then a
// projectile skill throws its particles towards the opponent, a contact skill (Charge) lunges and hits the opponent in
// its range, in front of it ; then a recovery. The Pokemon cannot move during the attack. A skill can be used again
// after its cooldown. The time of the battle stops during the hit stops (BattleStage).
class BattleSystem :
	public ska::System< ska::RequiredComponent<ska::PositionComponent, ska::HitboxComponent, ska::AnimationComponent, BattleComponent, FighterComponent, SkillsHolderComponent, ska::GraphicComponent>, ska::PossibleComponent<SkillComponent>>,
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
	// The active part of a contact skill : the Pokemon lunges forward (it hits at its start)
	static constexpr unsigned int LungeDuration = 140;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	bool onKeyInputEvent(ska::InputKeyEvent& event);
	bool fighting(ska::EntityId fighter) const;
	bool canAct(ska::EntityId fighter) const;
	void createSkill(unsigned int index, ska::EntityId from);
	void contact(unsigned int index, ska::EntityId entity, FighterComponent& fighter);
	// An area skill (the fighters of the other side around it) or a self skill (itself)
	void area(unsigned int index, ska::EntityId entity, FighterComponent& fighter);
	// The AI may dodge an attack coming (its script decides)
	void considerDodging(ska::EntityId entity, FighterComponent& fighter);
	void updateAttack(ska::EntityId entity, FighterComponent& fighter, unsigned int delta);
	// The fighter played with the keys (0 : none, ex : the AI plays both)
	ska::EntityId playedByKeys() const;
	// Each fighter fights the nearest one of the other side still standing
	void retarget();
	void decide(ska::EntityId entity, FighterComponent& fighter);
	ska::Point<float> center(ska::EntityId entity) const;

	CustomEntityManager& m_customEM;
	PokemonGameEventDispatcher& m_ged;
	BattleRules& m_rules;
	BattleStage& m_stage;
	const unsigned int m_blockSize;
	// Time of the battle (milliseconds)
	unsigned long m_time = 0;
	// A skill key pressed while the Pokemon of the player is busy (attack, stun) : used when it is free, if it comes soon
	static constexpr unsigned int InputBuffer = 200;
	int m_bufferedSkill = -1;
	unsigned long m_bufferedUntil = 0;
};
