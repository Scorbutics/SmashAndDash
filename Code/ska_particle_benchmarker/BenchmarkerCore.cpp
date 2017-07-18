#undef main
#include "BenchmarkerCore.h"
#include "../ska_core/Logging/Logger.h"
#include "../ska_core/Utils/TimeUtils.h"
#include "../ska_core/Exceptions/StateDiedException.h"
#include "../ska_particle/Impl/BoxParticleGenerator.h"
#include "../ska_particle/Impl/ColorParticleUpdater.h"
#include "../ska_particle/Impl/TimeParticleUpdater.h"
#include "../ska_particle/Impl/PhysicParticleUpdater.h"
#include "../ska_particle/Impl/ColoredRectGraphicParticleRenderer.h"
#include "Impl/BasicVelocityGenerator.h"
#include "Impl/BasicColorGenerator.h"
#include "Impl/ConsoleParticleCountRenderer.h"
#include "Impl/EulerAttractorParticleUpdater.h"
#include "Impl/SpreadingColorParticleEffectFactory.h"
#include "Impl/SpreadingParticleEffectFactory.h"
#include "Impl/SpreadingTextureParticleEffectFactory.h"
#include "Utils/FileUtils.h"
#include "Impl/SideBalancingParticleUpdater.h"

BenchmarkerCore::BenchmarkerCore() :
	m_window("ska Particle Benchmark", 1500, 900),
	m_particles(400, 70000) {
	{
		/*ska::Point<int> origin(950, 500);
		ska::Point<int> maxDistance(300, 0);
		m_particles.addGenerator<ska::BoxParticleGenerator>(origin, maxDistance);
		ska::Color cEnd(110, 10, 140, 255);
		ska::Color cStart(120, 220, 150, 255);
		m_particles.addGenerator<ska::BasicColorGenerator>(cStart, cEnd);
		ska::PolarPoint<float> initialVelocity(00.F, 1.57F);
		m_particles.addGenerator<ska::BasicVelocityGenerator>(initialVelocity, 0, 1);

		ska::PolarPoint<float> force;
		force.radius = 200.F;
		m_attractor = &m_particles.addUpdater<ska::EulerAttractorParticleUpdater>(origin, force);
		m_particles.addUpdater<ska::PhysicParticleUpdater>();
		static const auto lifetime = 20000;
		m_particles.addUpdater<ska::ColorParticleUpdater>(lifetime);
		m_particles.addUpdater<ska::TimeParticleUpdater>(lifetime);

		m_particles.addRenderer<ska::ColoredRectGraphicParticleRenderer>(m_window.getRenderer(), 1);
		m_particles.addRenderer<ska::ConsoleParticleCountRenderer>(500);*/

	}
	/**********************************************************/

	ska::SpreadingParticleSystemData effectData;
	effectData.lifetime = 2000;
	effectData.originalBoxSize.x = 15;
	effectData.origin.x = 750;
	effectData.origin.y = 450;
	effectData.initialVelocity.radius = 0.5F;
	effectData.initialVelocity.angle = 1.57F;
	effectData.spreading = 0.8F;
	effectData.density = 1;
	effectData.generationDelay = 350;
	effectData.maxParticles = 20;
	effectData.spritePath = ska::FileUtils::getExecutablePath() + "Sprites/Particles/4.png";
	auto& grassEffect = m_particleSystem.makeEffect<ska::SpreadingTextureParticleEffectFactory>(m_window.getRenderer(), effectData);
	grassEffect.addUpdater<ska::SideBalancingParticleUpdater>(effectData.origin, 1.F, 1.F);

}

void BenchmarkerCore::buildFireworks() {
	ska::Color cEnd(110, 30, 140, 255);
	ska::Color cStart(120, 220, 150, 255);
	ska::SpreadingParticleSystemData effectData;
	effectData.lifetime = 1500;
	effectData.origin.x = 750;
	effectData.origin.y = 450;
	effectData.cStart = cStart;
	effectData.cEnd = cEnd;
	effectData.initialVelocity.radius = 7.5F;
	effectData.initialVelocity.angle = 0.27F;
	effectData.spreading = M_PI;
	effectData.spreadingSlices = 10;
	effectData.density = 5;
	effectData.maxParticles = 50;
	m_particleSystem.makeEffect<ska::SpreadingColorParticleEffectFactory>(m_window.getRenderer(), effectData);

	effectData.lifetime = 2000;
	effectData.cEnd = ska::Color(110, 30, 40, 255);
	effectData.cStart = ska::Color(220, 130, 150, 255);
	effectData.initialVelocity.radius = 4.F;
	effectData.origin.x = 950;
	effectData.origin.y = 650;
	m_particleSystem.makeEffect<ska::SpreadingColorParticleEffectFactory>(m_window.getRenderer(), effectData);

	effectData.lifetime = 3500;
	effectData.cEnd = ska::Color(20, 30, 140, 255);
	effectData.cStart = ska::Color(150, 120, 220, 255);
	effectData.initialVelocity.radius = 3.F;
	effectData.origin.x = 1050;
	effectData.origin.y = 350;
	m_particleSystem.makeEffect<ska::SpreadingColorParticleEffectFactory>(m_window.getRenderer(), effectData);

	effectData.lifetime = 2700;
	effectData.cEnd = ska::Color(120, 30, 70, 255);
	effectData.cStart = ska::Color(250, 120, 120, 255);
	effectData.initialVelocity.radius = 5.F;
	effectData.origin.x = 200;
	effectData.origin.y = 250;
	m_particleSystem.makeEffect<ska::SpreadingColorParticleEffectFactory>(m_window.getRenderer(), effectData);
}

int BenchmarkerCore::onTerminate(ska::TerminateProcessException&) {
	return 0;
}

int BenchmarkerCore::onException(ska::GenericException& e) {
	std::cerr << "Uncaught exception : " << e.what() << std::endl;
	return 1;
}

void BenchmarkerCore::eventUpdate(const float ti) {
	m_inputListener.update();
	//m_attractor->move(m_inputListener.getMouseInput().getMousePos());
	//m_particles.refresh(static_cast<unsigned>(ti));
	m_particleSystem.refresh(static_cast<unsigned>(ti));
}

void BenchmarkerCore::graphicUpdate(const unsigned long) const {
	static const ska::Color black(0,0,0,255);
	//m_particles.display();
	m_particleSystem.display();
	m_fpsCalculator.getRenderable().display();
	m_window.setRenderColor(black);
	m_window.display();
}

void BenchmarkerCore::run() {
	unsigned long t0 = ska::TimeUtils::getTicks();
	const auto ti = ticksWanted();
	auto accumulator = ti;

	for (;;) {
		unsigned long t = ska::TimeUtils::getTicks();

		const auto ellapsedTime = t - t0;
		t0 = t;

		auto invAccu = 0U;

		accumulator += ellapsedTime;

		while (accumulator >= ti) {
			eventUpdate(ti);
			invAccu += static_cast<unsigned int>(ti);
			m_fpsCalculator.calculate(invAccu);
			accumulator -= ti;
		}

		graphicUpdate(ellapsedTime);
	}
}

float BenchmarkerCore::ticksWanted() const{
	static const unsigned int FPS = 60;
	static const float TICKS = 1000.F / FPS;
	return TICKS;
}


ska::GameApp& ska::GameApp::get() {
	static BenchmarkerCore app;
	return app;
}
