#include "ColoredRectGraphicParticleRenderer.h"
#include "../ParticleGroup.h"

ska::ColoredRectGraphicParticleRenderer::ColoredRectGraphicParticleRenderer(SDLRenderer& renderer, unsigned int particleDimensions) :
	m_renderer(renderer),
	m_particleDimensions(particleDimensions) {
}

void ska::ColoredRectGraphicParticleRenderer::display(const ParticleGroup& group) const {

	const auto& maxSize = group.getLength();

	if(m_particleDimensions == 1) {
		for (std::size_t i = 0; i < maxSize; i++) {
			m_renderer.drawColorPoint(group.color[i], group.pos[i]);
		}
	} else {
		//TODO voir pourquoi en -O3 ska::Rectangle r{0, 0, 5, 5}; crash
		ska::Rectangle r;
		r.h = m_particleDimensions;
		r.w = m_particleDimensions;
		for (std::size_t i = 0; i < maxSize; i++) {
			r.x = static_cast<int>(group.pos[i].x - r.w / 2);
			r.y = static_cast<int>(group.pos[i].y - r.h / 2);
			m_renderer.drawColorRect(group.color[i], r);
		}
	}
    
}
