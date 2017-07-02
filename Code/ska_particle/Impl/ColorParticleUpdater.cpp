#include "ColorParticleUpdater.h"
#include "../ParticleGroup.h"

ska::ColorParticleUpdater::ColorParticleUpdater(unsigned int maxLifetimeParticle) :
	m_maxLifetimeParticle(maxLifetimeParticle) {
}

void ska::ColorParticleUpdater::update(unsigned int dt, ParticleGroup& group) const {
	for(std::size_t i = 0; i < group.getLength(); i++) {
		const auto percentsLifetime = static_cast<float>(group.lifetime[i] * 100.F) / m_maxLifetimeParticle;
		group.color[i] = (group.endColor[i] * percentsLifetime) + group.startColor[i] * (100 - percentsLifetime);
		group.color[i].a = 255;
	}
}
