#include "Game/__internalConfig/LoggerConfig.h"
#include "SilhouetteSystem.h"
#include <algorithm>
#include "../World/WorldState.h"
#include "../../Utils/IDs.h"

SilhouetteSystem::SilhouetteSystem(ska::EntityManager& entityManager, WorldState& worldState) :
	System(entityManager),
	m_worldState(worldState) {
}

float SilhouetteSystem::covered(const ska::Rectangle& body, const long ground) const {
	const auto& world = m_worldState.getWorld();
	const auto blockSize = static_cast<int>(world.getBlockSize());
	const auto& profile = world.getCollisionProfile();
	if (blockSize <= 0 || body.w <= 0 || body.h <= 0) {
		return 0.F;
	}
	auto hidden = 0L;
	const auto x0 = std::max(0, body.x / blockSize);
	const auto y0 = std::max(0, body.y / blockSize);
	const auto x1 = std::min(static_cast<int>(world.getBlocksX()) - 1, (body.x + body.w - 1) / blockSize);
	const auto y1 = std::min(static_cast<int>(world.getBlocksY()) - 1, (body.y + body.h - 1) / blockSize);
	for (auto by = y0; by <= y1; by++) {
		for (auto bx = x0; bx <= x1; bx++) {
			// An object over the character there : a tile standing on a base line (its depth in the tileset) drawn after it
			// (as LayerDepthRenderable sorts it : TopLayerPriority + its layer + its base row x the block size)
			auto over = false;
			for (auto layer = std::size_t{ 1 }; layer < profile.layers() && !over; layer++) {
				const auto* block = profile.getBlock(layer, static_cast<std::size_t>(bx), static_cast<std::size_t>(by));
				if (block == nullptr || block->properties.base < 0) {
					continue;
				}
				const auto row = static_cast<long>(by + block->properties.base);
				over = TopLayerPriority + static_cast<long>(layer) + row * blockSize > ground;
			}
			if (!over) {
				continue;
			}
			const auto w = std::min(body.x + body.w, (bx + 1) * blockSize) - std::max(body.x, bx * blockSize);
			const auto h = std::min(body.y + body.h, (by + 1) * blockSize) - std::max(body.y, by * blockSize);
			hidden += static_cast<long>(std::max(0, w)) * std::max(0, h);
		}
	}
	return static_cast<float>(hidden) / static_cast<float>(body.w * body.h);
}

void SilhouetteSystem::refresh(unsigned int) {
	const auto blockSize = static_cast<int>(m_worldState.getWorld().getBlockSize());
	for (const auto entity : getEntities()) {
		auto& graphic = m_componentAccessor.get<ska::GraphicComponent>(entity);
		const auto& hitbox = m_componentAccessor.get<ska::HitboxComponent>(entity);
		const auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
		// High in the air : drawn over everything (GraphicSystem : the air layer)
		if (graphic.animatedSprites.empty() || position.z >= AirHeight) {
			graphic.silhouette = 0;
			continue;
		}
		// Sorted as GraphicSystem sorts it : its feet, from its position on the grid of its sortStep
		auto sortY = static_cast<long>(position.y);
		if (graphic.sortStep > 0) {
			const auto step = static_cast<long>(graphic.sortStep);
			sortY = (sortY >= 0 ? sortY / step : -((-sortY + step - 1) / step)) * step;
		}
		const auto ground = sortY + hitbox.yOffset + static_cast<long>(hitbox.height);
		// Its body : a block wide, the lower part of its drawing (the frames have margins), lifted by its height
		const auto origin = graphic.spriteOrigin(position.x, position.y, hitbox);
		const auto width = static_cast<int>(graphic.animatedSprites[0].getWidth());
		const auto height = static_cast<int>(graphic.animatedSprites[0].getHeight());
		const auto bodyHeight = height * 3 / 4;
		const auto body = ska::Rectangle{ origin.x + width / 2 - blockSize / 2, origin.y + height - bodyHeight - static_cast<int>(position.z), blockSize, bodyHeight };
		const auto part = covered(body, ground);
		const auto strength = std::clamp((part - ShowFrom) / (FullFrom - ShowFrom), 0.F, 1.F);
		graphic.silhouette = static_cast<unsigned char>(static_cast<float>(Opacity) * strength);
	}
}
