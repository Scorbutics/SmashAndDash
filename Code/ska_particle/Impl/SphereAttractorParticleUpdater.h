#pragma once
#include "../ParticleUpdater.h"
#include "../../ska_core/Point.h"

namespace ska {
	class SphereAttractorParticleUpdater :
		public ParticleUpdater {
	public:
		SphereAttractorParticleUpdater(Point<int>& origin, PolarPoint<float>& force);
		virtual ~SphereAttractorParticleUpdater() = default;

		void update(unsigned int dt, ParticleGroup& group) override;
		void move(const Point<int>& point);

	private:
		PolarPoint<float> m_force;
		Point<int> m_origin;
	};
}
