#include "Game/__internalConfig/LoggerConfig.h"
#include "SkillEntityCollisionResponse.h"
#include "Core/ECS/Basics/Physic/CollisionComponent.h"
#include "Game/Gameplay/Data/RawStatistics.h"
#include "Game/Gameplay/Fight/SkillComponent.h"
#include "Game/Gameplay/Fight/BattleComponent.h"
#include "Core/ECS/EntityManager.h"

SkillEntityCollisionResponse::SkillEntityCollisionResponse(PokemonGameEventDispatcher& ged, ska::EntityManager& em) :
	SubObserver<ska::CollisionEvent>(std::bind(&SkillEntityCollisionResponse::onCollisionEvent, this, std::placeholders::_1), ged),
	m_ged(ged), 
	m_entityManager(em) {
	m_ged.ska::Observable<ska::CollisionEvent>::addObserver(*this);
}


bool SkillEntityCollisionResponse::onCollisionEvent(ska::CollisionEvent& e) {
	if (e.collisionComponent == nullptr) {
		return false;
	}

	const auto col = *e.collisionComponent;
	if (!m_entityManager.hasComponent<SkillComponent>(col.origin) && !m_entityManager.hasComponent<SkillComponent>(col.target)) {
		return true;
	}

	SkillComponent* sc = nullptr;
	BattleComponent* bc = nullptr;
	auto skillEntity = static_cast<unsigned int>(-1);
	auto targettedEntity = static_cast<unsigned int>(-1);

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
	}

	return true;
}
