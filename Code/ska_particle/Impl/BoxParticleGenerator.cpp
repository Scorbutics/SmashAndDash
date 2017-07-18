#include <iostream>
#include "BoxParticleGenerator.h"
#include "../ParticleBuilder.h"
#include "../ParticleFactory.h"

ska::BoxParticleGenerator::BoxParticleGenerator(Point<int> origin, Point<int> maxDistance) :
	m_origin(origin),
	m_maxDistance(maxDistance) {
}

void ska::BoxParticleGenerator::build(ParticleBuilder& builder) const {
	if (m_maxDistance.x != 0 || m_maxDistance.y != 0) {
		builder.setRandomPosition(m_origin, m_maxDistance);
	} else {
		builder.setPosition(m_origin);
	}
}
