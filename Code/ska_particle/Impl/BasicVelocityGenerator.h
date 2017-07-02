#pragma once
#include "../ParticleGenerator.h"
#include "../../ska_core/Point.h"

namespace ska {
	class ParticleBuilder;
	class ParticleFactory;

	class BasicVelocityGenerator :
		public ParticleGenerator {
	public:
		explicit BasicVelocityGenerator(PolarPoint<float>& initialVelocityVector);
		virtual ~BasicVelocityGenerator() = default;

		void build(ParticleBuilder& builder) const override;

	private:
		Point<float> m_initialVelocityVector;
	};
}
