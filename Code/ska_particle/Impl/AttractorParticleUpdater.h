#pragma once
#include "../ParticleUpdater.h"
#include "../../ska_core/Point.h"

namespace ska {
	class AttractorParticleUpdater :
		public ParticleUpdater {
	public:
		AttractorParticleUpdater(Point<int>& origin, PolarPoint<float>& force);
		virtual ~AttractorParticleUpdater() = default;

		void update(unsigned int dt, ParticleGroup& group) override;
		void move(const Point<int>& point);

		PolarPoint<float> m_force;
		Point<int> m_origin;
	};
}
