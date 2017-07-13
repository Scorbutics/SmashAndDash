#pragma once
#include "../ParticleEffect.h"


namespace ska {
	class SDLRenderer;

	struct SpreadingParticleSystemData {
		SpreadingParticleSystemData() :
			cStart(255, 255, 255, 255),
			cEnd(0, 0, 0, 255),
			spreading(0.F),
			lifetime(1000),
			spreadingSlices(0),
			density(2),
			maxParticles(500),
			particleDimension(3),
			generationDelay(0) { }
		Point<int> origin;
		Color cStart;
		Color cEnd;
		ska::PolarPoint<float> initialVelocity;
		Point<int> originalBoxSize;
		float spreading;
		unsigned int lifetime;
		int spreadingSlices;
		unsigned int density;
		unsigned int maxParticles;
		unsigned int particleDimension;
		unsigned int generationDelay;
		std::string spritePath;
	};

	class SpreadingParticleEffectFactory {
	public:
		static std::unique_ptr<ParticleEffect> createEffect(SDLRenderer& renderer, const SpreadingParticleSystemData& data);

	private:
		SpreadingParticleEffectFactory() = default;
	};
}
