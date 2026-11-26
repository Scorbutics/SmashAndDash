#include "Game/__internalConfig/LoggerConfig.h"
#include "LeapTransition.h"
#include <algorithm>
#include <cmath>
#include <chipmunk/chipmunk.h>
#include "../../../CustomEntityManager.h"
#include "../../../World/WorldState.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "Core/Utils/TimeUtils.h"
#include "Physic/Space.h"

LeapTransition::LeapTransition(CustomEntityManager& em, WorldState& ws, ska::EntityId jumper, ska::EntityId awayFrom, int blocks) :
	ska::Task(std::bind(&LeapTransition::run, this, std::placeholders::_1)),
	m_entityManager(em),
	m_worldState(ws),
	m_jumper(jumper),
	m_awayFrom(awayFrom),
	m_blocks(blocks) {
}

void LeapTransition::start() {
	const auto& world = m_worldState.getWorld();
	const auto blockSize = static_cast<float>(world.getBlockSize());
	const auto& position = m_entityManager.getComponent<ska::PositionComponent>(m_jumper);
	const auto& other = m_entityManager.getComponent<ska::PositionComponent>(m_awayFrom);
	m_from = ska::Point<float>{ static_cast<float>(position.x), static_cast<float>(position.y) };
	m_to = m_from;

	// Away from the other one, on the axis where they are the farthest apart (0 : down, 1 : right, 2 : up, 3 : left)
	const auto dx = static_cast<float>(position.x - other.x);
	const auto dy = static_cast<float>(position.y - other.y);
	const auto horizontal = std::abs(dx) > std::abs(dy);
	const auto step = horizontal ? ska::Point<float>{ dx >= 0.F ? 1.F : -1.F, 0.F } : ska::Point<float>{ 0.F, dy >= 0.F ? 1.F : -1.F };
	const auto facing = horizontal ? (step.x > 0.F ? 3 : 1) : (step.y > 0.F ? 2 : 0);

	// Block by block, while the way is free (no wall, nor water, in the map)
	auto& space = m_worldState.getSpace();
	const auto block = ska::Point<float>{ std::round(m_from.x / blockSize), std::round(m_from.y / blockSize) };
	const auto margin = 4.;
	for (auto k = 1; k <= m_blocks; k++) {
		const auto x = block.x + step.x * static_cast<float>(k);
		const auto y = block.y + step.y * static_cast<float>(k);
		if (x < 0.F || y < 0.F || x >= static_cast<float>(world.getBlocksX()) || y >= static_cast<float>(world.getBlocksY())) {
			break;
		}
		const auto previousX = x - step.x;
		const auto previousY = y - step.y;
		const auto swept = cpBBNew(std::min(x, previousX) * blockSize + margin, std::min(y, previousY) * blockSize + margin,
			(std::max(x, previousX) + 1.F) * blockSize - margin, (std::max(y, previousY) + 1.F) * blockSize - margin);
		if (space.staticShapeIn(swept)) {
			break;
		}
		m_to = ska::Point<float>{ x * blockSize, y * blockSize };
	}
	if (m_entityManager.hasComponent<ska::AnimationComponent>(m_jumper)) {
		m_entityManager.getComponent<ska::AnimationComponent>(m_jumper).state = facing;
	}
	m_startedAt = ska::TimeUtils::getTicks();
}

bool LeapTransition::run(ska::Task&) {
	if (!m_entityManager.hasComponent<ska::PositionComponent>(m_jumper) || !m_entityManager.hasComponent<ska::PositionComponent>(m_awayFrom)) {
		return false;
	}
	if (!m_started) {
		m_started = true;
		start();
	}
	const auto t = std::min(1.F, static_cast<float>(ska::TimeUtils::getTicks() - m_startedAt) / static_cast<float>(Duration));
	// Fast then slower, over an arc
	const auto ease = 1.F - (1.F - t) * (1.F - t);
	CustomEntityManager::place(m_entityManager, m_worldState.getSpace(), m_jumper,
		ska::Point<float>{ m_from.x + (m_to.x - m_from.x) * ease, m_from.y + (m_to.y - m_from.y) * ease });
	auto& position = m_entityManager.getComponent<ska::PositionComponent>(m_jumper);
	position.z = static_cast<long>(std::lround(4.F * Height * t * (1.F - t)));
	if (m_entityManager.hasComponent<ska::MovementComponent>(m_jumper)) {
		auto& movement = m_entityManager.getComponent<ska::MovementComponent>(m_jumper);
		movement.vx = movement.vy = movement.vz = 0.F;
	}
	if (t >= 1.F) {
		position.z = 0;
		return false;
	}
	return true;
}
