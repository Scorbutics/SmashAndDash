#include "../../../ska/Graphic/GraphicComponent.h"
#include "../SkillComponent.h"
#include "BattleSystem.h"
#include "../../../ska/Physic/MovementComponent.h"
#include "../../../ska/Physic/HitboxComponent.h"
#include "../../../ska/Physic/ForceComponent.h"
#include "../../../ska/Inputs/Readers/IniReader.h"
#include "../../../ska/Inputs/InputContextManager.h"
#include "../../../ska/Graphic/SpritePath.h"

BattleSystem::BattleSystem(ska::EntityManager& em, const ska::InputContextManager& icm, const ska::EntityId player, const ska::EntityId opponent, const ska::IniReader& playerReader, const ska::IniReader& opponentReader) : 
System(em), m_icm(icm), 
m_player(player), m_opponent(opponent),
m_opponentReader(opponentReader), m_playerReader(playerReader) {

}

void BattleSystem::refresh() {
	const ska::InputToggleContainer& itc = m_icm.getToggles();
	const ska::InputActionContainer& iac = m_icm.getActions();

	for (ska::EntityId entityId : m_processed) {
		BattleComponent& bc = m_entityManager.getComponent<BattleComponent>(entityId);

		/* TODO SkillShots */
		if (iac[ska::InputAction::ShotSkill1]) {
			createSkill(0, entityId);
		} else if (iac[ska::InputAction::ShotSkill2]) {
		} else if (iac[ska::InputAction::ShotSkill3]) {
		} else if (iac[ska::InputAction::ShotSkill4]) {
		}
		
		if (bc.hp < 0) {
			m_entityManager.removeEntity(entityId);
		}

	}
}

void BattleSystem::createSkill(const unsigned int index, ska::EntityId from) {
	/* Ignore potential external entity */
	if (from != m_player && from != m_opponent) {
		return;
	}

	const ska::IniReader& reader = from == m_player ? m_playerReader : m_opponentReader;
	ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(from);
	ska::DirectionalAnimationComponent& dac = m_entityManager.getComponent<ska::DirectionalAnimationComponent>(from);
	SkillsHolderComponent& shc = m_entityManager.getComponent<SkillsHolderComponent>(from);

	if (index >= shc.skills.size()) {
		return;
	}


	int n = reader.getInt("Particle number");

	for (int i = 0; i < n; i++) {

		SkillComponent sc;
		//sc.noise = reader.getInt("Particle slope_noise");
		sc.speed = (float)reader.getInt("Particle speed");
		sc.damage = reader.getInt("Particle damage");
		sc.knockback = reader.getInt("Particle knockback");
		sc.noise = reader.getInt("Particle noise");

		SkillDescriptor& sd = shc.skills[index];
			
		ska::EntityId skill = m_entityManager.createEntity();

		ska::PositionComponent pc;
		ska::GraphicComponent gc;
		gc.sprite.resize(1);
		gc.sprite[0].load(ska::SpritePath::getInstance().getPath(SPRITEBANK_SKILL, sd.id), 2, 2, 2);
		m_entityManager.addComponent<ska::MovementComponent>(skill, ska::MovementComponent());
		m_entityManager.addComponent<SkillComponent>(skill, sc);
		m_entityManager.addComponent<ska::PositionComponent>(skill, pc);
		m_entityManager.addComponent<ska::HitboxComponent>(skill, ska::HitboxComponent());
		m_entityManager.addComponent<ska::GraphicComponent>(skill, gc);
	}
}

BattleSystem::~BattleSystem() {
}