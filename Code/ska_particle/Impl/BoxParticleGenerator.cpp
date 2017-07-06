#include "BoxParticleGenerator.h"
#include "../ParticleFactory.h"
#include "Utils/NumberUtils.h"

ska::BoxParticleGenerator::BoxParticleGenerator(Point<int> origin, Point<int> maxDistance) :
	m_origin(origin),
	m_maxDistance(maxDistance) {
}

void ska::BoxParticleGenerator::build(ParticleBuilder& builder) const {
	builder.setRandomPosition(m_origin, m_maxDistance);
}
