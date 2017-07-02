#include "BasicVelocityGenerator.h"
#include "../ParticleFactory.h"

ska::BasicVelocityGenerator::BasicVelocityGenerator(PolarPoint<float>& initialVelocityVector) {
	m_initialVelocityVector = ska::Point<float>::cartesian(initialVelocityVector.radius, initialVelocityVector.angle);
}

void ska::BasicVelocityGenerator::build(ParticleBuilder & builder) const {
	builder.setVelocity(m_initialVelocityVector);
}
