#include "BasicColorGenerator.h"
#include "../ParticleBuilder.h"
#include "../ParticleFactory.h"

ska::BasicColorGenerator::BasicColorGenerator(Color start, Color end) :
	m_start(start),
	m_end(end) {
}

void ska::BasicColorGenerator::build(ParticleBuilder& builder) const {
	builder.setStartColor(m_start);
	builder.setColor(m_start);
	builder.setEndColor(m_end);
}
