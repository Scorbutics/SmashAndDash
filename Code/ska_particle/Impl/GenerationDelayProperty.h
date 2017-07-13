#pragma once
#include <string>

namespace ska {
	class ParticleGroup;

	class GenerationDelayProperty {
	public:
		~GenerationDelayProperty() = default;

		static void applyProperty(ParticleGroup& group, unsigned int generationDelay);
	private:
		GenerationDelayProperty() = default;
	};
}
