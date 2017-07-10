#include "PhysicParticleUpdater.h"
#include "../ParticleGroup.h"
#include <iostream>

void ska::PhysicParticleUpdater::update(unsigned int, ParticleGroup & group) {
	const auto& maxSize = group.getLength();
	for (std::size_t i = 0; i < maxSize; i++) {
		auto& physics = group.physics[i];
		group.pos[i] += physics.velocity;
		physics.velocity += physics.acceleration;
		physics.acceleration = 0;
		
	}
}
