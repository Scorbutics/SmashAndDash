#include "BoxAttractorParticleUpdater.h"
#include "../ParticleGroup.h"
#include <iostream>

ska::BoxAttractorParticleUpdater::BoxAttractorParticleUpdater(Point<int>& origin, PolarPoint<float>& forceVector) :
	m_origin(origin) {
	m_force = forceVector;
}

void ska::BoxAttractorParticleUpdater::update(unsigned int, ParticleGroup & group) {
	const auto& maxSize = group.getLength();
	for (std::size_t i = 0; i < maxSize; i++) {
		ska::Point<float> diff(m_origin.x - group.pos[i].x, m_origin.y - group.pos[i].y) ;
		const auto& diffNorm = ska::NumberUtils::squareroot(diff.x * diff.x + diff.y * diff.y);
		
		diff.x = static_cast<float>((diff.x * m_force.radius) / diffNorm);
		diff.y = static_cast<float>((diff.y * m_force.radius) / diffNorm);
			
		group.physics[i].acceleration += diff;
	}
}

void ska::BoxAttractorParticleUpdater::move(const Point<int>& point) {
	m_origin = point;
}
