#include "SDLGraphicParticleRenderer.h"
#include "../ParticleGroup.h"

ska::SDLGraphicParticleRenderer::SDLGraphicParticleRenderer(SDLRenderer& renderer) :
	m_renderer(renderer) {
}

void ska::SDLGraphicParticleRenderer::display(const ParticleGroup& group) const {

	const auto& maxSize = group.getLength();

	//TODO voir pourquoi en -O3 ska::Rectangle r{0, 0, 5, 5}; crash
	/*ska::Rectangle r;
	r.h = 5;
	r.w = 5;
	for (std::size_t i = 0; i < maxSize; i++) {
		r.x = group.pos[i].x - r.w / 2;
		r.y = group.pos[i].y - r.h / 2;
		m_renderer.drawColorRect(group.color[i], r);
	}*/

    for (std::size_t i = 0; i < maxSize; i++) {
		m_renderer.drawColorPoint(group.color[i], group.pos[i]);
	}
}
