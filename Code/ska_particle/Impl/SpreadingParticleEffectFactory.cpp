#include "../ParticleBuilder.h"
#include "SpreadingParticleEffectFactory.h"
#include "BasicColorGenerator.h"
#include "ColorParticleUpdater.h"
#include "TimeParticleUpdater.h"
#include "PhysicParticleUpdater.h"
#include "ColoredRectGraphicParticleRenderer.h"
#include "BoxParticleGenerator.h"
#include "BasicVelocityGenerator.h"


std::unique_ptr<ska::ParticleEffect> ska::SpreadingParticleEffectFactory::createEffect(SDLRenderer& renderer, const SpreadingParticleSystemData & data) {
	auto particleSystem = std::make_unique<ParticleEffect>(data.density, data.maxParticles);

	particleSystem->addGenerator<BoxParticleGenerator>(data.origin, data.originalBoxSize);
	particleSystem->addGenerator<BasicVelocityGenerator>(data.initialVelocity, data.spreading, data.spreadingSlices);

	particleSystem->addUpdater<TimeParticleUpdater>(data.lifetime);
	particleSystem->addUpdater<PhysicParticleUpdater>();

	return std::move(particleSystem);
}
