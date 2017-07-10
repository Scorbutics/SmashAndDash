#include "../ParticleGroup.h"
#include "SideBalancingParticleUpdater.h"
#include <iostream>
#include "Utils/RectangleUtils.h"

ska::SideBalancingParticleUpdater::SideBalancingParticleUpdater(Point<int> origin, float amplitude, float speed) :
	m_amplitude(amplitude),
	m_speed(speed),
	m_origin(origin) {
}

void ska::SideBalancingParticleUpdater::update(unsigned int ellapsedTime, ParticleGroup& group) {
	const auto& maxSize = group.getLength();
	for (std::size_t i = 0; i < maxSize; i++) {
		const auto& currentAngleDirection = PolarPoint<float>::polar(group.pos[i].x - m_origin.x, group.pos[i].y - m_origin.y);

		const auto& forceAmplitude = m_amplitude * NumberUtils::cosinus((group.lifetime[i] / (ellapsedTime * 2 * M_PI ))  * m_speed);
		const auto& forceAngle = currentAngleDirection.angle + (M_PI / 2);

		const auto& force = Point<float>::cartesian(forceAmplitude, forceAngle);

		group.pos[i] += force;
	}
}



