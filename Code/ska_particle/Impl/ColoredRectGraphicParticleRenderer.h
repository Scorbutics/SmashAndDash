#pragma once
#include "../ParticleRenderer.h"
#include "Draw/SDLRenderer.h"

namespace ska {
	class ColoredRectGraphicParticleRenderer :
		public ParticleRenderer {
	public:
		explicit ColoredRectGraphicParticleRenderer(SDLRenderer& renderer, unsigned int particleDimensions);
		virtual ~ColoredRectGraphicParticleRenderer() = default;

		void display(const ParticleGroup& group) const override;

	private:
		SDLRenderer& m_renderer;
		unsigned int m_particleDimensions;
	};
}
