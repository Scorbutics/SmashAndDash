#pragma once
#include "../ParticleGenerator.h"
#include "../../ska_core/Point.h"

namespace ska {
	class ParticleFactory;

	class BoxParticleGenerator : 
		public ParticleGenerator {
	public:
		BoxParticleGenerator(Point<int> origin, Point<int> maxDistance);
		virtual ~BoxParticleGenerator() = default;

		void build(ParticleBuilder& builder) const override;
	
	private:
		Point<int> m_origin;
		Point<int> m_maxDistance;
		
	};
}
