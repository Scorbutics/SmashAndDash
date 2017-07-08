#pragma once
#include "../ParticleGenerator.h"
#include "../../ska_core/Point.h"

namespace ska {
	class ParticleBuilder;
	class ParticleFactory;

	class BasicVelocityGenerator :
		public ParticleGenerator {
	public:
		BasicVelocityGenerator(PolarPoint<float> initialVelocityVector, float spreading, unsigned int slices);
		virtual ~BasicVelocityGenerator() = default;

		void build(ParticleBuilder& builder) const override;

	private:
		PolarPoint<float> m_initialVelocityVector;
		float m_spreading;
		unsigned int m_slices;
	};
}
