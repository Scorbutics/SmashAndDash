#include "SkillEntityCollisionResponse.h"
#include "../../ska/Physic/CollisionComponent.h"
#include "../../Gameplay/Data/RawStatistics.h"
#include "../../ska/Physic/WorldCollisionComponent.h"
#include "../../ska/Physic/System/CollisionSystem.h"
#include "../../Gameplay/Fight/SkillComponent.h"
#include "../../Gameplay/Fight/BattleComponent.h"
#include "../../ska/ECS/EntityManager.h"

SkillEntityCollisionResponse::SkillEntityCollisionResponse(ska::CollisionSystem& colSys, PokemonGameEventDispatcher& ged, ska::EntityManager& em) :
EntityCollisionResponse(bind(&SkillEntityCollisionResponse::onEntityCollision, this, std::placeholders::_1), ged, em), 
m_collisionSystem(colSys), 
m_ged(ged) {
	m_ged.ska::Observable<ska::CollisionEvent>::addObserver(*this);
}


bool SkillEntityCollisionResponse::onEntityCollision(ska::CollisionEvent& e) {
	if (e.collisionComponent == nullptr) {
		return false;
	}
	auto col = *e.collisionComponent;
	if (!m_entityManager.hasComponent<SkillComponent>(col.origin) && !m_entityManager.hasComponent<SkillComponent>(col.target)) {
		EntityCollisionResponse::onEntityCollision(e);
		return true;
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
		return true;
	}

	if (sc->battler != targettedEntity) {
		bc->hp -= sc->damage;
		StatisticsChangeEvent sce(targettedEntity, *bc, sc->battler);
		m_ged.ska::Observable<StatisticsChangeEvent>::notifyObservers(sce);
		m_collisionSystem.scheduleDeferredRemove(skillEntity);
	}

	return true;
}

/*
void SkillCollisionSystem::handleWorldCollision(ska::WorldCollisionComponent& col, ska::EntityId e) {


	if (!m_entityManager.hasComponent<SkillComponent>(e)) {
		CollisionSystem::handleWorldCollision(col, e);
	}
}*/

SkillEntityCollisionResponse::~SkillEntityCollisionResponse() {
	m_ged.ska::Observable<ska::CollisionEvent>::removeObserver(*this);
}