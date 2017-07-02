#include "AttractorParticleUpdater.h"
#include "../ParticleGroup.h"

ska::AttractorParticleUpdater::AttractorParticleUpdater(Point<int>& origin, PolarPoint<float>& forceVector) : 
	m_origin(origin) {
	m_force = forceVector;
}

void ska::AttractorParticleUpdater::update(unsigned int dt, ParticleGroup & group) const {
	for (std::size_t i = 0; i < group.getLength(); i++) {
		auto cartesianVector = m_origin - group.pos[i];
		auto vector = ska::PolarPoint<float>::polar(cartesianVector.x, cartesianVector.y);
		vector.radius = m_force.radius;
		group.physics[i].acceleration += ska::Point<float>::cartesian(vector.radius, vector.angle);
	}
}
