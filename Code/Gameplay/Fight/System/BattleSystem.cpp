
#include "BattleSystem.h"
#include "../SkillComponent.h"
#include "../../../ska/Inputs/Readers/IniReader.h"
#include "../../../ska/Inputs/InputContextManager.h"
#include "../../CustomEntityManager.h"

BattleSystem::BattleSystem(CustomEntityManager& em, const ska::InputContextManager& icm, const ska::EntityId player, const ska::EntityId opponent, const ska::IniReader& playerReader, const ska::IniReader& opponentReader) : 
System(em), m_icm(icm), 
m_player(player), m_opponent(opponent),
m_opponentReader(opponentReader), m_playerReader(playerReader),
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
			/* TODO end fight */
			m_entityManager.removeEntity(entityId);
		}

	}
}

void BattleSystem::createSkill(const unsigned int index, ska::EntityId from) {
	/* Ignore potential external entity */
	if (from != m_player && from != m_opponent) {
		return;
	}
	const ska::InputRangeContainer& irc = m_icm.getRanges();
	const ska::InputRange& mousePos = irc[ska::InputRangeType::MousePos];
	const ska::IniReader& reader = from == m_player ? m_playerReader : m_opponentReader;
	ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(from);
	ska::DirectionalAnimationComponent& dac = m_entityManager.getComponent<ska::DirectionalAnimationComponent>(from);
	SkillsHolderComponent& shc = m_entityManager.getComponent<SkillsHolderComponent>(from);

	if (index >= shc.skills.size()) {
		return;
	}


	int n = reader.getInt("Particle number");

	for (int i = 0; i < n; i++) {
		ska::EntityId skill = m_customEM.createSkill(reader, shc, i);
		
		m_entityManager.addComponent<ska::PositionComponent>(skill, pc);
		SkillComponent& sc = m_entityManager.getComponent<SkillComponent>(skill);
		sc.target = mousePos;
	}
}

BattleSystem::~BattleSystem() {
}