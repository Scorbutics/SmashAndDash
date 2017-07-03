#pragma once
#include "../ParticleRenderer.h"
#include "Draw/SDLRenderer.h"

namespace ska {
	class SDLGraphicParticleRenderer :
		public ParticleRenderer {
	public:
		explicit SDLGraphicParticleRenderer(SDLRenderer& renderer);
		virtual ~SDLGraphicParticleRenderer() = default;

		void display(const ParticleGroup& group) const override;

	private:
		SDLRenderer& m_renderer;
	};
}
