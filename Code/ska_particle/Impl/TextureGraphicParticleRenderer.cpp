#include <iostream>
#include "TextureGraphicParticleRenderer.h"
#include "../ParticleGroup.h"

ska::TextureGraphicParticleRenderer::TextureGraphicParticleRenderer() {
}

void ska::TextureGraphicParticleRenderer::display(const ParticleGroup& group) const {
	const auto& maxSize = group.getLength();
	for (std::size_t i = 0; i < maxSize; i++) {
		group.appearance.render(group.pos[i].x, group.pos[i].y);
	}
}
