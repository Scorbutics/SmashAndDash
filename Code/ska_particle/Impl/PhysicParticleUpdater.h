#pragma once
#include "../ParticleUpdater.h"

namespace ska {
	class PhysicParticleUpdater :
		public ParticleUpdater {
	public:
		PhysicParticleUpdater() = default;
		virtual ~PhysicParticleUpdater() = default;

		void update(unsigned int dt, ParticleGroup& group) override;
	};
}
