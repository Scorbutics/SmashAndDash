#include "TextureAlphaParticleUpdater.h"
#include "../ParticleGroup.h"

ska::TextureAlphaParticleUpdater::TextureAlphaParticleUpdater(unsigned int maxLifetimeParticle) :
	m_maxLifetimeParticle(maxLifetimeParticle) {
}

void ska::TextureAlphaParticleUpdater::update(unsigned int, ParticleGroup& group) {
	const auto& maxSize = group.getLength();
	if (maxSize > 0) {
		const auto percentsLifetime = static_cast<float>(group.lifetime[0]) / m_maxLifetimeParticle;
		const auto remainingPercentsLifetime = (1 - percentsLifetime);
		group.appearance.setAlpha(remainingPercentsLifetime * 255);
	}

}
