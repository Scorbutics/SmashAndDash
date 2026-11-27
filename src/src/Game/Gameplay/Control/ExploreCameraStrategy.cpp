#include "Game/__internalConfig/LoggerConfig.h"
#include "ExploreCameraStrategy.h"
#include <algorithm>
#include <cmath>
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/Utils/TimeUtils.h"
#include "GridMovementComponent.h"

ExploreCameraStrategy::ExploreCameraStrategy(ska::EntityManager& entityManager) :
	m_entityManager(entityManager) {
}

std::pair<ska::Rectangle, const ska::EntityId*> ExploreCameraStrategy::focus(const std::unordered_set<ska::EntityId>& processed) {
	const auto it = processed.begin();
	if (it == processed.end()) {
		return { ska::Rectangle{}, nullptr };
	}
	// The entity in the set (not a copy : the pointer is used after this function returns)
	const auto& entity = *it;
	const auto& position = m_entityManager.getComponent<ska::PositionComponent>(entity);

	// The lead : where it walks to, reached smoothly (the time of the game : frozen in a pause)
	const auto now = ska::TimeUtils::getTicks();
	const auto elapsed = m_last == 0 ? 0.F : static_cast<float>(std::min(now - m_last, 100u));
	m_last = now;
	auto target = ska::Point<float>{};
	if (m_entityManager.hasComponent<GridMovementComponent>(entity)) {
		const auto& grid = m_entityManager.getComponent<GridMovementComponent>(entity);
		target = { grid.velocity.x * LeadTime, grid.velocity.y * LeadTime };
		const auto length = std::sqrt(target.x * target.x + target.y * target.y);
		if (length > Lead) {
			target = { target.x * Lead / length, target.y * Lead / length };
		}
	}
	const auto rate = 1.F - std::exp(-elapsed / LeadSmoothing);
	m_lead.x += (target.x - m_lead.x) * rate;
	m_lead.y += (target.y - m_lead.y) * rate;

	return { ska::Rectangle{ static_cast<int>(position.x + std::lround(m_lead.x)), static_cast<int>(position.y + std::lround(m_lead.y)), 0, 0 }, &entity };
}
