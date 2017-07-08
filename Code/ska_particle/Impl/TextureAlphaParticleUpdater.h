#pragma once
#include "../ParticleUpdater.h"

namespace ska {
	class TextureAlphaParticleUpdater :
		public ParticleUpdater {
	public:
		explicit TextureAlphaParticleUpdater(unsigned int maxLifetimeParticle);
		virtual ~TextureAlphaParticleUpdater() = default;

		void update(unsigned int dt, ParticleGroup& group) override;
	private:
		const unsigned int m_maxLifetimeParticle;
	};
}
