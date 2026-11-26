#include "Game/__internalConfig/LoggerConfig.h"
#include "PokeballTransition.h"
#include "../../../CustomEntityManager.h"
#include "../../../PokeballComponent.h"
#include "../../../System/PokeballSystem.h"
#include "Core/Utils/TimeUtils.h"

PokeballTransition::PokeballTransition(CustomEntityManager& em, Path path, ska::Point<float>& landing) :
	ska::Task(std::bind(&PokeballTransition::run, this, std::placeholders::_1)),
	m_entityManager(em),
	m_path(std::move(path)),
	m_landing(landing) {
}

bool PokeballTransition::run(ska::Task&) {
	const auto now = ska::TimeUtils::getTicks();
	if (!m_thrown) {
		m_thrown = true;
		const auto [from, to] = m_path();
		m_landing = to;
		m_ball = PokeballSystem::Throw(m_entityManager, from, to, false);
		m_thrownAt = now;
		return true;
	}
	// Until it opened, and the Pokemon comes out
	if (!m_entityManager.hasComponent<PokeballComponent>(m_ball) || now - m_thrownAt >= MaxDuration) {
		return false;
	}
	if (!m_entityManager.getComponent<PokeballComponent>(m_ball).opened()) {
		return true;
	}
	if (m_openedAt == 0) {
		m_openedAt = now;
	}
	return now - m_openedAt < AppearDelay;
}
