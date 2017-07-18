#pragma once
#include <cstddef>

namespace ska {
	struct Color;
	class ParticleGroup;
	class ParticleBuilder;

	class ParticleFactory {
	public:
		ParticleFactory(ParticleBuilder& builder);
		~ParticleFactory() = default;
		ska::ParticleBuilder& prepareNextParticles(ParticleGroup& group, unsigned int density, unsigned int ellapsedTime);
		void updateCurrentActiveCounter();

	private:
	    bool hasAvailableNextParticles(unsigned int ellapsedTime) const;

        bool m_needsCounterUpdate;
		ParticleBuilder& m_builder;
		ParticleGroup* m_group;
		std::size_t m_lastActiveIndex;
	};
}
