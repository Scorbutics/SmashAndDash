#include "ParticleEffect.h"

ska::ParticleEffect::ParticleEffect(unsigned int density, std::size_t systemSize) :
	m_density(density),
	m_group(systemSize) {
}

void ska::ParticleEffect::refresh(unsigned int dt) {
	auto& builder = m_factory.createNextParticles(m_group, m_density);
	for (const auto& e : m_generators) {
		e->build(builder);
	}

	for(const auto& u : m_updaters) {
		u->update(dt, m_group);
	}
	m_factory.updateCurrentActiveCounter();
}

void ska::ParticleEffect::display() const {
	for (const auto& u : m_renderers) {
		u->display(m_group);
	}
}
