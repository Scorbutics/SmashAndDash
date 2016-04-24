#include "SkillCollisionSystem.h"
#include "../../ska/Physic/CollisionComponent.h"
#include "../../Gameplay/Fight/SkillComponent.h"
#include "../../Gameplay/Fight/BattleComponent.h"

SkillCollisionSystem::SkillCollisionSystem(ska::World& w, ska::EntityManager& em) : ska::CollisionSystem(w, em) {

}

void SkillCollisionSystem::handleEntityCollision(ska::CollisionComponent& col) {
	CollisionSystem::handleEntityCollision(col);
	
	SkillComponent* sc = nullptr;
	BattleComponent* bc = nullptr;

	/* Skill collision with a battle entity => Statistics modification */
	if (m_entityManager.hasComponent<SkillComponent>(col.origin) && m_entityManager.hasComponent<BattleComponent>(col.target)) {
		sc = &m_entityManager.getComponent<SkillComponent>(col.origin);
		bc = &m_entityManager.getComponent<BattleComponent>(col.target);
	}

	if (sc != nullptr && m_entityManager.hasComponent<SkillComponent>(col.target) && m_entityManager.hasComponent<BattleComponent>(col.origin)) {
		sc = &m_entityManager.getComponent<SkillComponent>(col.target);
		bc = &m_entityManager.getComponent<BattleComponent>(col.origin);
	}

	if (sc == nullptr) {
		/* Not a skillshot or not on an entity, but absorbs the skill */
		if (bc != nullptr) {
			m_entityManager.removeComponent<SkillComponent>(col.origin);
			m_entityManager.removeComponent<SkillComponent>(col.target);
		}
		return;
	}
	

	bc->hp -= sc->damage;

}

SkillCollisionSystem::~SkillCollisionSystem() {

}