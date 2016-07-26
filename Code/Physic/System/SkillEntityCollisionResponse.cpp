#include "SkillEntityCollisionResponse.h"
#include "../../ska/Physic/CollisionComponent.h"
#include "../../Gameplay/Data/RawStatistics.h"
#include "../../ska/Physic/WorldCollisionComponent.h"
#include "../../Gameplay/Fight/SkillComponent.h"
#include "../../Gameplay/Fight/BattleComponent.h"

SkillEntityCollisionResponse::SkillEntityCollisionResponse(ska::CollisionSystem& colSys, ska::EntityManager& em) : 
ska::EntityCollisionResponse(std::bind(&SkillEntityCollisionResponse::onEntityCollision, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), colSys, em){
	m_collisionSystem.ska::EntityCollisionObservable::addObserver(*this);
}


void SkillEntityCollisionResponse::onEntityCollision(const ska::CollisionEvent& e, ska::CollisionComponent& col, const ska::CollidableComponent& cc) {
	if (!m_entityManager.hasComponent<SkillComponent>(col.origin) && !m_entityManager.hasComponent<SkillComponent>(col.target)) {
		EntityCollisionResponse::onEntityCollision(e, col, cc);
		return;
	}


	SkillComponent* sc = nullptr;
	BattleComponent* bc = nullptr;
	ska::EntityId skillEntity;
	ska::EntityId targettedEntity;

	/* Skill collision with a battle entity => Statistics modification */
	if (m_entityManager.hasComponent<SkillComponent>(col.origin) && m_entityManager.hasComponent<BattleComponent>(col.target)) {
		skillEntity = col.origin;
		targettedEntity = col.target;
		sc = &m_entityManager.getComponent<SkillComponent>(skillEntity);
		bc = &m_entityManager.getComponent<BattleComponent>(col.target);
	}

	if (sc != nullptr && m_entityManager.hasComponent<SkillComponent>(col.target) && m_entityManager.hasComponent<BattleComponent>(col.origin)) {
		skillEntity = col.target;
		targettedEntity = col.origin;
		sc = &m_entityManager.getComponent<SkillComponent>(skillEntity);
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

	if (sc->battler != targettedEntity) {
		bc->hp -= sc->damage;
		notifyObservers(targettedEntity, *bc, sc->battler);
		m_collisionSystem.scheduleDeferredRemove(skillEntity);
	}

}

/*
void SkillCollisionSystem::handleWorldCollision(ska::WorldCollisionComponent& col, ska::EntityId e) {


	if (!m_entityManager.hasComponent<SkillComponent>(e)) {
		CollisionSystem::handleWorldCollision(col, e);
	}
}*/

SkillEntityCollisionResponse::~SkillEntityCollisionResponse() {
	m_collisionSystem.ska::EntityCollisionObservable::removeObserver(*this);
}