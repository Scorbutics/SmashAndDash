#include "AttractorParticleUpdater.h"
#include "../ParticleGroup.h"

ska::AttractorParticleUpdater::AttractorParticleUpdater(Point<int>& origin, PolarPoint<float>& forceVector) :
	m_origin(origin) {
	m_force = forceVector;
}

void ska::AttractorParticleUpdater::update(unsigned int dt, ParticleGroup & group) {
	for (std::size_t i = 0; i < group.getLength(); i++) {

        const auto& cartesianVector = m_origin - group.pos[i];
        const auto& vec = ska::PolarPoint<float>::polar(cartesianVector.x, cartesianVector.y);
        m_force.angle = vec.angle;

		group.physics[i].acceleration += ska::Point<float>::cartesian(m_force.radius, m_force.angle);
	}
}

void ska::AttractorParticleUpdater::move(const Point<int>& point) {
	m_origin = point;
}
