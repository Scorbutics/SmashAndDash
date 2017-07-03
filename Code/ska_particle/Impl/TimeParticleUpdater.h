#pragma once
#include "../ParticleUpdater.h"

namespace ska {
	class TimeParticleUpdater :
		public ParticleUpdater {
	public:
		explicit TimeParticleUpdater(unsigned int maxLifetime);
		virtual ~TimeParticleUpdater() = default;

		void update(unsigned int dt, ParticleGroup& group) override;
		unsigned m_maxLifetime;
	};
}

