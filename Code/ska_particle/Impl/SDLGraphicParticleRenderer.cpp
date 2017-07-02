#include "SDLGraphicParticleRenderer.h"
#include "../ParticleGroup.h"

ska::SDLGraphicParticleRenderer::SDLGraphicParticleRenderer(SDLRenderer& renderer) :
	m_renderer(renderer) {
}

void ska::SDLGraphicParticleRenderer::display(const ParticleGroup& group) const {
	for (std::size_t i = 0; i < group.getLength(); i++) {
		m_renderer.drawColorPoint(group.color[i], group.pos[i]);
	}
}
