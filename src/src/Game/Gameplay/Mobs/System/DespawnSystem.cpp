#include <algorithm>
#include <cmath>
#include "Game/__internalConfig/LoggerConfig.h"
#include "DespawnSystem.h"
#include "../../Control/GridMovementComponent.h"
#include "../../World/WorldState.h"
#include "../../World/Chunks/DespawnRules.h"
#include "Graphic/System/CameraSystem.h"

void DespawnSystem::refresh(const unsigned int ellapsedTime) {
	m_elapsed += ellapsedTime;
	if (m_elapsed < Delay) {
		return;
	}
	m_elapsed = 0;
	const auto player = m_worldState.playerPosition();
	if (!player.has_value()) {
		return;
	}
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	const auto& camera = m_worldState.getCamera().getDisplay();
	// On the screen, or just out of it
	const auto margin = static_cast<int>(2.F * blockSize);

	// By id : the same dice for the same ones every time
	auto wilds = std::vector<ska::EntityId>{ getEntities().begin(), getEntities().end() };
	std::sort(wilds.begin(), wilds.end());
	auto leaving = std::vector<ska::EntityId>{};
	auto staying = std::vector<std::pair<float, ska::EntityId>>{};
	for (const auto entity : wilds) {
		const auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
		const auto distance = std::hypot(static_cast<float>(position.x) - player->x, static_cast<float>(position.y) - player->y) / blockSize;
		const auto seen = position.x >= camera.x - margin && position.x <= camera.x + camera.w + margin &&
			position.y >= camera.y - margin && position.y <= camera.y + camera.h + margin;
		auto engaged = false;
		if (m_entities.hasComponent<GridMovementComponent>(entity)) {
			const auto& grid = m_entities.getComponent<GridMovementComponent>(entity);
			engaged = grid.seeing && grid.reaction == GridMovementComponent::Reaction::Approach;
		}
		if (chunks::Despawns(distance, seen, engaged, rand() % 100)) {
			leaving.push_back(entity);
		} else if (!seen && !engaged) {
			staying.emplace_back(distance, entity);
		}
	}
	// Too many of them : the farthest out of the screen
	auto excess = static_cast<int>(wilds.size() - leaving.size()) - chunks::LiveCap;
	std::sort(staying.begin(), staying.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
	for (auto i = std::size_t{ 0 }; excess > 0 && i < staying.size(); i++, excess--) {
		leaving.push_back(staying[i].second);
	}
	for (const auto entity : leaving) {
		SLOG(ska::LogLevel::Debug) << "Wild pokemon " << entity << " leaves : far from the player";
		m_worldState.removeCharacter(entity);
	}
}
