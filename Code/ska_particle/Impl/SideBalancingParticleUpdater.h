#pragma once
#include "../ParticleUpdater.h"

namespace ska {
	class SideBalancingParticleUpdater :
		public ParticleUpdater {
	public:
		SideBalancingParticleUpdater(Point<int> origin, float amplitude, float speed);
		virtual ~SideBalancingParticleUpdater() = default;

		void update(unsigned int dt, ParticleGroup& group) override;

	private:
		float m_amplitude;
		float m_speed;
		Point<int> m_origin;
	};
}
