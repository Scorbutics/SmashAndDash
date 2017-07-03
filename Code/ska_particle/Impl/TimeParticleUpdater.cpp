#include "TimeParticleUpdater.h"
#include "../ParticleGroup.h"

ska::TimeParticleUpdater::TimeParticleUpdater(unsigned int maxLifetime) :
	m_maxLifetime(maxLifetime) {

}

void ska::TimeParticleUpdater::update(unsigned int dt, ParticleGroup& group) {
	for(std::size_t i = 0; i < group.getLength(); i++) {
		auto& lifetime = group.lifetime[i];
		lifetime += dt;
		if(static_cast<unsigned>(lifetime) >= m_maxLifetime) {
			group.kill(i);
		}
	}
}
