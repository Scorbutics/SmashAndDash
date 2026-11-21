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
#include "../SkillsHolderComponent.h"
#include "AI/System/IADefinedMovementSystem.h"
#include "../SkillComponent.h"
#include "Graphic/GraphicComponent.h"

namespace ska {
	class InputContextManager;
	class IniReader;
}

class CustomEntityManager;
class BattleRules;

// Real time battle : the Pokemon of the player uses its skills with the keys 1 to 4, the wild Pokemon uses its skills
// when its AI (script, see BattleRules) decides it. A skill can be used again after its cooldown.
class BattleSystem :
	public ska::System< ska::RequiredComponent<ska::PositionComponent, ska::HitboxComponent, ska::AnimationComponent, BattleComponent, SkillsHolderComponent, ska::GraphicComponent>, ska::PossibleComponent<SkillComponent>>,
	public ska::SubObserver<ska::InputKeyEvent> {
public:
	BattleSystem(CustomEntityManager& em, PokemonGameEventDispatcher& ged, BattleRules& rules, const ska::EntityId pokemon, const ska::EntityId opponent, unsigned int blockSize);
	BattleSystem(const BattleSystem&) = delete;

	BattleSystem& operator=(const BattleSystem&) = delete;

	// Uses a skill when it is ready : returns false otherwise
	bool useSkill(unsigned int index, ska::EntityId from);
	bool skillReady(unsigned int index, ska::EntityId from) const;
	virtual ~BattleSystem();

protected:
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	static constexpr unsigned int DecisionDelay = 500;

	bool onKeyInputEvent(ska::InputKeyEvent& event);
	bool fighting(ska::EntityId fighter) const;
	void createSkill(const unsigned int index, ska::EntityId from);
	void decide();
	void decide(ska::EntityId fighter, ska::EntityId target);
	std::array<unsigned long, 4>& lastUses(ska::EntityId fighter);
	const std::array<unsigned long, 4>& lastUses(ska::EntityId fighter) const;

	CustomEntityManager& m_customEM;
	BattleRules& m_rules;
	const ska::EntityId m_pokemon;
	const ska::EntityId m_opponent;
	const unsigned int m_blockSize;
	// Time of the battle (milliseconds), time of the last use of each skill (0 : never used)
	unsigned long m_time = 0;
	unsigned long m_nextDecision = DecisionDelay;
	std::array<unsigned long, 4> m_pokemonUses {};
	std::array<unsigned long, 4> m_opponentUses {};
	// Development : the AI also plays the Pokemon of the player (environment variable SAD_BATTLE_AUTOPLAY)
	const bool m_autoplay;
};
