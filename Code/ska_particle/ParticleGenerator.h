#pragma once

namespace ska {
	class ParticleBuilder;

	class ParticleGenerator {
	protected:
		ParticleGenerator() = default;

	public:
		virtual void build(ParticleBuilder& builder) const = 0;
		virtual ~ParticleGenerator() = default;
	};
}
