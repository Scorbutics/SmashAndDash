#pragma once
#include "../ParticleUpdater.h"
#include "../../ska_core/Point.h"

namespace ska {
	class EulerAttractorParticleUpdater :
		public ParticleUpdater {
	public:
		EulerAttractorParticleUpdater(Point<int>& origin, PolarPoint<float>& force);
		virtual ~EulerAttractorParticleUpdater() = default;

		void update(unsigned int dt, ParticleGroup& group) override;
		void move(const Point<int>& point);

		PolarPoint<float> m_force;
		Point<int> m_origin;
	};
}
