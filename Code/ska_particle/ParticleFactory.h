#pragma once
#include <vector>
#include "../ska_core/Point.h"

namespace ska {
	struct Color;
	class ParticleGroup;

	class ParticleBuilder {
	public:
		ParticleBuilder();
		~ParticleBuilder() = default;

		void target(ParticleGroup& group, std::size_t particleIndexStart, std::size_t particleIndexEnd);

		const ParticleBuilder& setAcceleration(Point<float> acceleration) const;
		const ParticleBuilder& setVelocity(Point<float> velocity) const;
		const ParticleBuilder& setPosition(Point<float> position) const;
		const ParticleBuilder& setStartColor(const Color& c) const;
		const ParticleBuilder& setEndColor(const Color& c) const;

	private:
		ParticleGroup* m_group;
		unsigned m_indexStart;
		unsigned m_indexEnd;
	};

	class ParticleFactory {
	public:
		ParticleFactory() = default;
		~ParticleFactory() = default;
		ska::ParticleBuilder& createNextParticles(ParticleGroup& group, unsigned int density);

	private:
		ParticleBuilder m_builder;
	};
}
