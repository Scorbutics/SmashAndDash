#include "Game/__internalConfig/LoggerConfig.h"
#include "TallGrassSystem.h"
#include <algorithm>
#include <cmath>
#include "../World/WorldState.h"

TallGrassSystem::TallGrassSystem(ska::EntityManager& entityManager, WorldState& worldState) :
	System(entityManager),
	m_worldState(worldState) {
}

bool TallGrassSystem::grass(int x, int y) const {
	const auto& world = m_worldState.getWorld();
	if (x < 0 || y < 0 || static_cast<std::size_t>(x) >= world.getBlocksX() || static_cast<std::size_t>(y) >= world.getBlocksY()) {
		return false;
	}
	const auto* block = world.getCollisionProfile().getBlock(1, static_cast<std::size_t>(x), static_cast<std::size_t>(y));
	return block != nullptr && (block->properties.bitMask & TilePropertyGrass) == TilePropertyGrass;
}

void TallGrassSystem::refresh(unsigned int) {
	const auto blockSize = static_cast<int>(m_worldState.getWorld().getBlockSize());
	if (blockSize <= 0) {
		return;
	}
	for (const auto entity : getEntities()) {
		auto& graphic = m_componentAccessor.get<ska::GraphicComponent>(entity);
		const auto& hitbox = m_componentAccessor.get<ska::HitboxComponent>(entity);
		if (graphic.animatedSprites.empty()) {
			graphic.cropBottom = 0;
			continue;
		}
		// Its footprint : a block wide and high, its bottom where its drawing ends (its feet), centred on its drawing. From
		// its logic position (drawnPosition belongs to the drawing : sampled here, it would break its smoothing)
		const auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
		const auto origin = graphic.spriteOrigin(position.x, position.y, hitbox);
		const auto width = static_cast<int>(graphic.animatedSprites[0].getWidth());
		const auto height = static_cast<int>(graphic.animatedSprites[0].getHeight());
		const auto left = origin.x + width / 2 - blockSize / 2;
		const auto bottom = origin.y + height - 1;
		const auto top = bottom - blockSize;
		// The part of it on tall grass (its blocks : 4 at most)
		auto covered = 0;
		for (auto by = top / blockSize; by * blockSize < bottom; by++) {
			for (auto bx = left / blockSize; bx * blockSize < left + blockSize; bx++) {
				if (!grass(bx, by)) {
					continue;
				}
				const auto w = std::min(left + blockSize, (bx + 1) * blockSize) - std::max(left, bx * blockSize);
				const auto h = std::min(bottom, (by + 1) * blockSize) - std::max(top, by * blockSize);
				covered += std::max(0, w) * std::max(0, h);
			}
		}
		const auto crop = static_cast<int>(std::lround(static_cast<double>(Depth) * covered / (blockSize * blockSize))) - static_cast<int>(position.z);
		graphic.cropBottom = std::clamp(crop, 0, Depth);
	}
}
