#include "BasicVelocityGenerator.h"
#include "../ParticleFactory.h"
#include "../ParticleBuilder.h"

ska::BasicVelocityGenerator::BasicVelocityGenerator(PolarPoint<float> initialVelocityVector, float spreading, unsigned int slices) :
	m_spreading(spreading),
	m_initialVelocityVector(initialVelocityVector),
	m_slices(slices) {
}

void ska::BasicVelocityGenerator::build(ParticleBuilder & builder) const {
	builder.setVelocity(m_initialVelocityVector, m_spreading, m_slices);
}
