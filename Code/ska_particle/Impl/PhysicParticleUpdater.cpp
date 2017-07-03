#include "PhysicParticleUpdater.h"
#include "../ParticleGroup.h"

void ska::PhysicParticleUpdater::update(unsigned int dt, ParticleGroup & group) {
	for (std::size_t i = 0; i < group.getLength(); i++) {
		auto& physics = group.physics[i];
		group.pos[i] += physics.velocity;
		physics.velocity += physics.acceleration;
		physics.acceleration = 0;
	}
}
