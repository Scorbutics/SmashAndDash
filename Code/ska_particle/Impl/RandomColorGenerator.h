#pragma once
#include "../../ska_core/Draw/Color.h"
#include "../ParticleGenerator.h"

namespace ska {
	class ParticleBuilder;
	class ParticleFactory;

	class RandomColorGenerator :
		public ParticleGenerator {
	public:
		RandomColorGenerator(Color min, Color max);
		virtual ~RandomColorGenerator() = default;

		void build(ParticleBuilder& builder) const override;

	private:
		static const Color& brightestColor(const Color& c1, const Color& c2);

		Color m_min; 
		Color m_max;

	};
}
