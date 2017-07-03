#include "ParticleSystem.h"

ska::ParticleSystem::ParticleSystem(unsigned int density) :
	m_density(density) {
}

void ska::ParticleSystem::refresh(unsigned int dt) {
	auto& builder = m_factory.createNextParticles(m_group, m_density);
	for (const auto& e : m_generators) {
		e->build(builder);
	}
	

	for(const auto& u : m_updaters) {
		u->update(dt, m_group);
	}
	m_factory.updateCurrentActiveCounter();
}

void ska::ParticleSystem::display() const {
	for (const auto& u : m_renderers) {
		u->display(m_group);
	}
}
