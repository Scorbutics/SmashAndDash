#include "SpreadingColorParticleEffectFactory.h"
#include "ColoredRectGraphicParticleRenderer.h"
#include "SpreadingParticleEffectFactory.h"
#include "ColorParticleUpdater.h"
#include "BasicColorGenerator.h"


std::unique_ptr<ska::ParticleEffect> ska::SpreadingColorParticleEffectFactory::createEffect(SDLRenderer& renderer, const SpreadingParticleSystemData & data) {
	auto particleSystem = SpreadingParticleEffectFactory::createEffect(renderer, data);

	particleSystem->addGenerator<BasicColorGenerator>(data.cStart, data.cEnd);
	particleSystem->addUpdater<ColorParticleUpdater>(data.lifetime);
	particleSystem->addRenderer<ColoredRectGraphicParticleRenderer>(renderer, data.particleDimension);
	return std::move(particleSystem);
}
