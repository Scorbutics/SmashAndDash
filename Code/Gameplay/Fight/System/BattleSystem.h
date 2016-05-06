#pragma once
#include <unordered_set>
#include "../../../ska/ECS/System.h"
#include "../../../ska/Physic/PositionComponent.h"
#include "../../../ska/Graphic/DirectionalAnimationComponent.h"
#include "../BattleComponent.h"
#include "../SkillsHolderComponent.h"

namespace ska {
	class InputContextManager;
	class IniReader;
}

class CustomEntityManager;

class BattleSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, ska::DirectionalAnimationComponent, BattleComponent, SkillsHolderComponent> {
public:
	BattleSystem(CustomEntityManager& em, const ska::InputContextManager& icm, const ska::EntityId pokemon, const ska::EntityId opponent, const ska::IniReader& pokemonReader, const ska::IniReader& opponentReader);
	void refresh() override;
	void createSkill(const unsigned int index, ska::EntityId from);
	virtual ~BattleSystem();

private:
	CustomEntityManager& m_customEM;
	const ska::InputContextManager& m_icm;
	const ska::EntityId m_pokemon;
	const ska::EntityId m_opponent;
	const ska::IniReader& m_pokemonReader;
	const ska::IniReader& m_opponentReader;
};