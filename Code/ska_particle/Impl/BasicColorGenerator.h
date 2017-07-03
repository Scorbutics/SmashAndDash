#pragma once
#include "../../ska_core/Draw/Color.h"
#include "../ParticleGenerator.h"

namespace ska {
	class ParticleBuilder;
	class ParticleFactory;

	class BasicColorGenerator :
		public ParticleGenerator {
	public:
		BasicColorGenerator(Color start, Color end);
		virtual ~BasicColorGenerator() = default;

		void build(ParticleBuilder& builder) const override;

	private:
		Color m_start; 
		Color m_end;

	};
}
