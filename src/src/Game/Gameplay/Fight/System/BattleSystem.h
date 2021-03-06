#pragma once
#include <unordered_set>
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

class BattleSystem : public ska::System< ska::RequiredComponent<ska::PositionComponent, ska::HitboxComponent, ska::AnimationComponent, BattleComponent, SkillsHolderComponent, ska::GraphicComponent>, ska::PossibleComponent<SkillComponent>> {
public:
	BattleSystem(CustomEntityManager& em, const ska::EntityId pokemon, const ska::EntityId opponent, const ska::IniReader& pokemonReader, const ska::IniReader& opponentReader);
	BattleSystem(const BattleSystem&) = delete;

	BattleSystem& operator=(const BattleSystem&) = delete;

	void createSkill(const unsigned int index, ska::EntityId from);
	virtual ~BattleSystem();

protected:
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	CustomEntityManager& m_customEM;
	const ska::EntityId m_pokemon;
	const ska::EntityId m_opponent;
	const ska::IniReader& m_pokemonReader;
	const ska::IniReader& m_opponentReader;
};
