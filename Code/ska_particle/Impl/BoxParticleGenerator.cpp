#include "BoxParticleGenerator.h"
#include "../ParticleFactory.h"
#include "Utils/NumberUtils.h"

ska::BoxParticleGenerator::BoxParticleGenerator(Point<int> origin, Point<int> maxDistance) :
	m_origin(origin),
	m_maxDistance(maxDistance) {
}

void ska::BoxParticleGenerator::build(ParticleBuilder& builder) const {
	builder
		.setPosition(m_origin +  Point<int>(
			NumberUtils::random(- m_maxDistance.x, m_maxDistance.x),
			NumberUtils::random(- m_maxDistance.y, m_maxDistance.y)));
}
