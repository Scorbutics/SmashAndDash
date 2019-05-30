#pragma once
#include "Core/ECS/EntityManager.h"

enum BattleEventType {
	BATTLE_START,
	BATTLE_END
};

namespace ska {
	class CameraSystem;
}

class BattleEvent {
public:
	BattleEvent(BattleEventType b, ska::CameraSystem& camSys, const ska::EntityId& battler, const ska::EntityId& opponent, ska::EntityManager& entityManager) :
		m_eventType(b),
		m_cameraSystem(camSys),
		m_battler(battler),
		m_opponent(opponent),
		m_entityManager(entityManager) {
	}

	const BattleEventType& getEventType() const {
		return m_eventType;
	}

	~BattleEvent() = default;


	ska::CameraSystem& getCameraSystem() const {
		return m_cameraSystem;
	}



	const ska::EntityId& getBattler() const {
		return m_battler;
	}



	const ska::EntityId& getOpponent() const {
		return m_opponent;
	}



	ska::EntityManager& getEntityManager() const {
		return m_entityManager;
	}

	void operator=(const BattleEvent&) = delete;

private:
	BattleEventType m_eventType;
	ska::CameraSystem& m_cameraSystem;
	const ska::EntityId& m_battler;
	const ska::EntityId& m_opponent;
	ska::EntityManager& m_entityManager;

};
