
#include "BattleSystem.h"
#include "../SkillComponent.h"
#include "Inputs/Readers/IniReader.h"
#include "Inputs/InputContextManager.h"
#include "../../CustomEntityManager.h"
#include "Utils/PhysicUtils.h"
#include "Utils/NumberUtils.h"
#include "Graphic/GraphicComponent.h"

BattleSystem::BattleSystem(CustomEntityManager& em, const ska::InputContextManager& playerICM, const ska::InputContextManager& opponentICM, const ska::EntityId player, const ska::EntityId opponent, const ska::IniReader& playerReader, const ska::IniReader& opponentReader) :
System(em), m_customEM(em),
m_playerICM(playerICM), m_opponentICM(opponentICM),
m_pokemon(player), m_opponent(opponent),
m_pokemonReader(playerReader), m_opponentReader(opponentReader) {

}

void BattleSystem::refresh(unsigned int ellapsedTime) {
	const auto& processed = getEntities();
	for (ska::EntityId entityId : processed) {
		const ska::InputActionContainer& iac = entityId == m_pokemon ? m_playerICM.getActions() : m_opponentICM.getActions();
		//BattleComponent& bc = m_entityManager.getComponent<BattleComponent>(entityId);

		/* TODO SkillShots */
		if (iac[ska::InputAction::ShotSkill1]) {
			createSkill(0, entityId);
		} else if (iac[ska::InputAction::ShotSkill2]) {
			createSkill(1, entityId);
		} else if (iac[ska::InputAction::ShotSkill3]) {
			createSkill(2, entityId);
		} else if (iac[ska::InputAction::ShotSkill4]) {
			createSkill(3, entityId);
		}

	}
}

void BattleSystem::createSkill(const unsigned int index, ska::EntityId from) {
	/* Ignore potential external entity */
	if (from != m_pokemon && from != m_opponent) {
		return;
	}
	//const ska::InputRangeContainer& irc = from == m_pokemon ? m_playerICM.getRanges() : m_opponentICM.getRanges();
	//const ska::InputRange& mousePos = irc[ska::InputRangeType::MousePos];
	auto& pc = m_componentAccessor.get<ska::PositionComponent>(from);
	auto& hc = m_componentAccessor.get<ska::HitboxComponent>(from);
	auto& dac = m_componentAccessor.get<ska::DirectionalAnimationComponent>(from);
	SkillsHolderComponent& shc = m_componentAccessor.get<SkillsHolderComponent>(from);

	if (index >= shc.skills.size()) {
		return;
	}

	auto skillStartPos = ska::PositionComponent::getFrontPosition(pc, hc, dac);


	auto n = shc.skills[index].particleNumber;

	for (auto i = 0; i < n; i++) {
		auto skill = m_customEM.createSkill(shc, i);

		const auto& scPtr = m_componentPossibleAccessor.get<SkillComponent>(skill);
		if(scPtr == nullptr) {
			continue;
		}

		auto& gcSkill = m_componentAccessor.get<ska::GraphicComponent>(skill);
		auto& sc = *scPtr;
		sc.origin = skillStartPos - ska::Point<int>(gcSkill.sprite[0].getWidth() / 2, gcSkill.sprite[0].getHeight() / 2);
		m_componentAccessor.add<ska::PositionComponent>(skill, sc.origin);

		const auto target = from == m_pokemon ? m_opponent : m_pokemon;
		const auto& pcTarget = m_componentAccessor.get<ska::PositionComponent>(target);
		const auto& hcTarget = m_componentAccessor.get<ska::HitboxComponent>(target);
		const auto pTarget = ska::Point<int>(hcTarget.xOffset, hcTarget.yOffset) + ska::Point<int>(pcTarget);

		sc.target = pTarget;
		sc.battler = from;
	}
}

BattleSystem::~BattleSystem() {
}
