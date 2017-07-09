#include "../ParticleGroup.h"
#include "SphereAttractorParticleUpdater.h"

ska::SphereAttractorParticleUpdater::SphereAttractorParticleUpdater(Point<int>& origin, PolarPoint<float>& forceVector) :
	m_origin(origin) {
	m_force = forceVector;
}

void ska::SphereAttractorParticleUpdater::update(unsigned int, ParticleGroup & group) {
	const auto& maxSize = group.getLength();
	for (std::size_t i = 0; i < maxSize; i++) {
		m_force.angle = static_cast<float>(NumberUtils::arctan(m_origin.x - group.pos[i].x, m_origin.y - group.pos[i].y));
		group.physics[i].acceleration += ska::Point<float>::cartesian(m_force.radius, m_force.angle);
	}
}

void ska::SphereAttractorParticleUpdater::move(const Point<int>& point) {
	m_origin = point;
}


