#pragma once
#include "../ParticleUpdater.h"
#include "../../ska_core/Point.h"

namespace ska {
	class BoxAttractorParticleUpdater :
		public ParticleUpdater {
	public:
		BoxAttractorParticleUpdater(Point<int>& origin, PolarPoint<float>& force);
		virtual ~BoxAttractorParticleUpdater() = default;

		void update(unsigned int dt, ParticleGroup& group) override;
		void move(const Point<int>& point);

		PolarPoint<float> m_force;
		Point<int> m_origin;
	};
}
