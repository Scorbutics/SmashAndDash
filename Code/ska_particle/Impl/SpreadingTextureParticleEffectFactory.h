#pragma once
#include "../ParticleEffect.h"

namespace ska {
	struct SpreadingParticleSystemData;

	class SpreadingTextureParticleEffectFactory {
	public:
		~SpreadingTextureParticleEffectFactory() = default;
		static std::unique_ptr<ParticleEffect> createEffect(SDLRenderer& renderer, const SpreadingParticleSystemData& data);
		
	private:
		SpreadingTextureParticleEffectFactory() = default;
	};
}
