#pragma once
#include <unordered_set>
#include "../../../ska/ECS/System.h"
#include "../../../ska/Physic/PositionComponent.h"
#include "../../../ska/Physic/HitboxComponent.h"
#include "../../../ska/Graphic/DirectionalAnimationComponent.h"
#include "../BattleComponent.h"
#include "../SkillsHolderComponent.h"

namespace ska {
	class InputContextManager;
	class IniReader;
}

class CustomEntityManager;

class BattleSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, ska::HitboxComponent, ska::DirectionalAnimationComponent, BattleComponent, SkillsHolderComponent> {
public:
	BattleSystem(CustomEntityManager& em, const ska::InputContextManager& playerICM, const ska::InputContextManager& opponentICM, const ska::EntityId pokemon, const ska::EntityId opponent, const ska::IniReader& pokemonReader, const ska::IniReader& opponentReader);
	BattleSystem(const BattleSystem&) = delete;

	BattleSystem& operator=(const BattleSystem&) = delete;

	void createSkill(const unsigned int index, ska::EntityId from);
	virtual ~BattleSystem();

protected:
	virtual void refresh() override;

private:
	CustomEntityManager& m_customEM;
	const ska::InputContextManager& m_playerICM;
	const ska::InputContextManager& m_opponentICM;
	const ska::EntityId m_pokemon;
	const ska::EntityId m_opponent;
	const ska::IniReader& m_pokemonReader;
	const ska::IniReader& m_opponentReader;
};