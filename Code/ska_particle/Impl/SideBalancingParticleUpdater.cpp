#include "../ParticleGroup.h"
#include "SideBalancingParticleUpdater.h"
#include <iostream>

ska::SideBalancingParticleUpdater::SideBalancingParticleUpdater(Point<int> origin, float amplitude, float speed) :
	m_amplitude(amplitude),
	m_speed(speed),
	m_origin(origin) {
}

void ska::SideBalancingParticleUpdater::update(unsigned int ellapsedTime, ParticleGroup& group) {
	const auto& maxSize = group.getLength();
	for (std::size_t i = 0; i < maxSize; i++) {
		const auto& amplitude = m_amplitude * NumberUtils::cosinus((2 * M_PI / (group.lifetime[i]))  * m_speed);
		const auto& currentAngleDirection = ska::PolarPoint<float>::polar(group.pos[i].x - m_origin.x, group.pos[i].y - m_origin.y);
		const auto& forceAngle = currentAngleDirection.angle + (M_PI / 2);
		const auto& force = ska::Point<float>::cartesian(amplitude, forceAngle);
		std::cout << amplitude << "\t" << forceAngle << "\t original angle : " << currentAngleDirection.angle << "\n";
		
		group.physics[i].acceleration += (force /*+ repulsiveForce*/);
	}
	std::cout << std::endl;
}



