#include "ColorParticleUpdater.h"
#include "../ParticleGroup.h"

ska::ColorParticleUpdater::ColorParticleUpdater(unsigned int maxLifetimeParticle) :
	m_maxLifetimeParticle(maxLifetimeParticle) {
}

void ska::ColorParticleUpdater::update(unsigned int, ParticleGroup& group) {
	const auto& maxSize = group.getLength();
	for (std::size_t i = 0; i < maxSize; i++) {
		const auto percentsLifetime = static_cast<float>(group.lifetime[i]) / m_maxLifetimeParticle;
		const auto remainingPercentsLifetime = (1 - percentsLifetime);

        auto& color = group.color[i];
		color.r = static_cast<uint8_t>(group.endColor[i].r * percentsLifetime + group.startColor[i].r * remainingPercentsLifetime);
		color.g = static_cast<uint8_t>(group.endColor[i].g * percentsLifetime + group.startColor[i].g * remainingPercentsLifetime);
		color.b = static_cast<uint8_t>(group.endColor[i].b * percentsLifetime + group.startColor[i].b * remainingPercentsLifetime);
	}
}
