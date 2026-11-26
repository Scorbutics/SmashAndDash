#include "Game/__internalConfig/LoggerConfig.h"
#include "ArrivalSystem.h"
#include <cmath>
#include "Core/Utils/RectangleUtils.h"

namespace {
	// The row of the frames of a direction in a charset (as ska::WalkAnimationStateMachine)
	int DirectionRow(int direction) {
		static const int rows[8] = { 2, 1, 0, 3, 7, 5, 4, 6 };
		return direction >= 0 && direction < 8 ? rows[direction] : 0;
	}
}

ArrivalSystem::ArrivalSystem(ska::EntityManager& entityManager, Arrived arrived) :
	System(entityManager),
	m_arrived(std::move(arrived)) {
}

std::vector<ArrivalSystem::Rustle> ArrivalSystem::rustles() const {
	auto out = std::vector<Rustle>{};
	for (const auto entity : getEntities()) {
		const auto& arrival = m_componentAccessor.get<ArrivalComponent>(entity);
		if (arrival.hideout.has_value() && !arrival.moving()) {
			out.push_back({ *arrival.hideout, arrival.elapsed });
		}
	}
	return out;
}

void ArrivalSystem::refresh(unsigned int ellapsedTime) {
	// Made wild Pokemon after the loop : a new entity moves the components (no reference into them meanwhile)
	auto arrived = std::vector<ArrivalComponent>{};
	for (const auto entity : getEntities()) {
		auto& arrival = m_componentAccessor.get<ArrivalComponent>(entity);
		auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
		auto& graphic = m_componentAccessor.get<ska::GraphicComponent>(entity);
		if (arrival.elapsed > arrival.wait + arrival.duration) {
			continue; // arrived, removed at the end of the frame
		}
		arrival.elapsed += ellapsedTime;
		const auto t = arrival.progress();

		auto ground = 0.F;
		auto height = 0.F;
		auto scale = 1.F;
		switch (arrival.kind) {
		case ArrivalComponent::Kind::Land:
			// A glide : fast then slower, lower and lower (it brakes before the ground)
			ground = 1.F - (1.F - t) * (1.F - t);
			height = arrival.toHeight + (arrival.fromHeight - arrival.toHeight) * (1.F - t) * (1.F - t);
			break;
		case ArrivalComponent::Kind::Emerge:
			// A hop out of the hideout, growing out of it
			ground = t;
			height = arrival.arc * 4.F * t * (1.F - t);
			scale = 0.4F + 0.6F * std::min(1.F, t * 1.5F);
			break;
		case ArrivalComponent::Kind::TakeOff:
			// Slow then faster, higher and higher
			ground = t * t;
			height = arrival.fromHeight + (arrival.toHeight - arrival.fromHeight) * t * t;
			break;
		}
		position.x = static_cast<long>(std::lround(arrival.from.x + (arrival.to.x - arrival.from.x) * ground));
		position.y = static_cast<long>(std::lround(arrival.from.y + (arrival.to.y - arrival.from.y) * ground));
		position.z = static_cast<long>(std::lround(height));

		graphic.effect = ska::RenderEffect{};
		graphic.effect.scaleX = graphic.effect.scaleY = scale;
		// Not seen while it waits in its hideout
		graphic.effect.alpha = arrival.moving() ? 255 : 0;
		if (!graphic.animatedSprites.empty()) {
			auto& texture = graphic.animatedSprites[0];
			const auto direction = ska::RectangleUtils::getDirectionFromPos(arrival.from, arrival.to);
			auto offset = texture.getOffsetBase();
			offset.y = static_cast<int>(texture.getHeight()) * DirectionRow(direction);
			texture.setOffset(offset);
			texture.setDelay(arrival.kind == ArrivalComponent::Kind::Emerge ? 120 : 70);
			texture.stop(false);
		}

		if (arrival.elapsed >= arrival.wait + arrival.duration) {
			arrival.elapsed = arrival.wait + arrival.duration + 1;
			if (arrival.kind != ArrivalComponent::Kind::TakeOff) {
				arrived.push_back(arrival);
			}
			graphic.animatedSprites.clear();
			scheduleDeferredRemove(entity);
		}
	}
	if (m_arrived) {
		for (const auto& arrival : arrived) {
			m_arrived(arrival);
		}
	}
}
