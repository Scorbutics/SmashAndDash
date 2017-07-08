#include <iostream>

#include "ConsoleParticleCountRenderer.h"
#include "Utils/TimeUtils.h"
#include "../ParticleGroup.h"


ska::ConsoleParticleCountRenderer::ConsoleParticleCountRenderer(unsigned int frequency) :
	m_frequency(frequency) {
}

void ska::ConsoleParticleCountRenderer::display(const ParticleGroup & group) const {
	static unsigned int lastTime = ska::TimeUtils::getTicks();
	if(ska::TimeUtils::getTicks() - lastTime >= m_frequency) {
		std::cout << "Particle count : " << group.getLength() << std::endl;
		lastTime = ska::TimeUtils::getTicks();
	}
}
