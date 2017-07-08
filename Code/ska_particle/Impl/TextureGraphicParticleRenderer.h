#pragma once
#include "../ParticleRenderer.h"
#include "Graphic/Texture.h"

namespace ska {
	class TextureGraphicParticleRenderer :
		public ParticleRenderer {
	public:
		TextureGraphicParticleRenderer();
		virtual ~TextureGraphicParticleRenderer() = default;

		void display(const ParticleGroup& group) const override;

	};
}
