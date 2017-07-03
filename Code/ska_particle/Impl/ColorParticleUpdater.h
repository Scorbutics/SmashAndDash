#pragma once
#include "../ParticleUpdater.h"

namespace ska {
	class ColorParticleUpdater :
		public ParticleUpdater {
	public:
		explicit ColorParticleUpdater(unsigned int maxLifetimeParticle);
		virtual ~ColorParticleUpdater() = default;

		void update(unsigned int dt, ParticleGroup& group) override;
	private:
		const unsigned int m_maxLifetimeParticle;
	};
}
