
#include "BattleSystem.h"
#include "../SkillComponent.h"
#include "../../../ska/Inputs/Readers/IniReader.h"
#include "../../../ska/Inputs/InputContextManager.h"
#include "../../CustomEntityManager.h"
#include "../../../ska/Utils/PhysicUtils.h"
#include "../../../ska/Utils/NumberUtils.h"
#include "../../../ska/Graphic/GraphicComponent.h"

BattleSystem::BattleSystem(CustomEntityManager& em, const ska::InputContextManager& icm, const ska::EntityId player, const ska::EntityId opponent, const ska::IniReader& playerReader, const ska::IniReader& opponentReader) : 
System(em), m_icm(icm), 
m_pokemon(player), m_opponent(opponent),
m_opponentReader(opponentReader), m_pokemonReader(playerReader),
m_customEM(em) {

}

void BattleSystem::refresh() {
	
	const ska::InputActionContainer& iac = m_icm.getActions();
	
	for (ska::EntityId entityId : m_processed) {
		BattleComponent& bc = m_entityManager.getComponent<BattleComponent>(entityId);

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
		
		if (bc.hp < 0) {
			/* TODO end fight + move this out of the loop */
			m_entityManager.removeEntity(entityId);
		}

	}
}

void BattleSystem::createSkill(const unsigned int index, ska::EntityId from) {
	/* Ignore potential external entity */
	if (from != m_pokemon && from != m_opponent) {
		return;
	}
	const ska::InputRangeContainer& irc = m_icm.getRanges();
	const ska::InputRange& mousePos = irc[ska::InputRangeType::MousePos];
	ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(from);
	ska::HitboxComponent& hc = m_entityManager.getComponent<ska::HitboxComponent>(from);
	ska::DirectionalAnimationComponent& dac = m_entityManager.getComponent<ska::DirectionalAnimationComponent>(from);
	SkillsHolderComponent& shc = m_entityManager.getComponent<SkillsHolderComponent>(from);

	if (index >= shc.skills.size()) {
		return;
	}

	ska::Point<int> skillStartPos = ska::PositionComponent::getFrontPosition(pc, hc, dac);
	

	int n = shc.skills[index].particleNumber;

	for (int i = 0; i < n; i++) {
		ska::EntityId skill = m_customEM.createSkill(shc, i);
		
		
		SkillComponent& sc = m_entityManager.getComponent<SkillComponent>(skill);
		ska::GraphicComponent& gcSkill = m_entityManager.getComponent<ska::GraphicComponent>(skill);
		sc.origin = skillStartPos - ska::Point<int>(gcSkill.sprite[0].getWidth() / 2, gcSkill.sprite[0].getHeight() / 2);
		m_entityManager.addComponent<ska::PositionComponent>(skill, sc.origin);
		ska::PositionComponent& pcTarget = m_entityManager.getComponent<ska::PositionComponent>(from == m_pokemon ? m_opponent : m_pokemon);
		sc.target = pcTarget;
	}
}

BattleSystem::~BattleSystem() {
}