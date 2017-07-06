#undef main
#include "BenchmarkerCore.h"
#include "../ska_core/Logging/Logger.h"
#include "../ska_core/Utils/TimeUtils.h"
#include "../ska_core/Exceptions/StateDiedException.h"
#include "../ska_particle/Impl/BoxParticleGenerator.h"
#include "../ska_draw/Graphic/Texture.h"
#include "../ska_particle/Impl/ColorParticleUpdater.h"
#include "../ska_particle/Impl/TimeParticleUpdater.h"
#include "../ska_particle/Impl/PhysicParticleUpdater.h"
#include "../ska_particle/Impl/AttractorParticleUpdater.h"
#include "../ska_particle/Impl/SDLGraphicParticleRenderer.h"
#include "Impl/BasicVelocityGenerator.h"
#include "Impl/BasicColorGenerator.h"

BenchmarkerCore::BenchmarkerCore() :
	m_window("ska Particle Benchmark", 800, 600),
	m_particles(200) {

	ska::Point<int> origin(400, 300);
	ska::Point<int> maxDistance(200, 0);
	m_particles.addGenerator<ska::BoxParticleGenerator>(origin, maxDistance);
	ska::Color cEnd(10, 10, 40, 255);
	ska::Color cStart(120, 120, 250, 255);
	m_particles.addGenerator<ska::BasicColorGenerator>(cStart, cEnd);
	ska::PolarPoint<float> initialVelocity(10.F, 1.57F);
	m_particles.addGenerator<ska::BasicVelocityGenerator>(initialVelocity);

	ska::PolarPoint<float> force;
	force.radius = 1.0F;
	force.angle = 5.9F;
	m_attractor = &m_particles.addUpdater<ska::AttractorParticleUpdater>(origin, force);
	m_particles.addUpdater<ska::PhysicParticleUpdater>();
	static const auto lifetime = 10000;
	m_particles.addUpdater<ska::ColorParticleUpdater>(lifetime);
	m_particles.addUpdater<ska::TimeParticleUpdater>(lifetime);

	m_particles.addRenderer<ska::SDLGraphicParticleRenderer>(m_window.getRenderer());
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
	m_attractor->move(m_inputListener.getMouseInput().getMousePos());
	m_particles.refresh(static_cast<unsigned>(ti));
}

void BenchmarkerCore::graphicUpdate(const unsigned long) const {
	static const ska::Color black(0,0,0,255);
	m_particles.display();
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

		accumulator += ellapsedTime;


		auto accIt = 0;
		while (accumulator >= ti) {
			accIt += ti;
			m_fpsCalculator.calculate(accIt);
			eventUpdate(ti);
			accumulator -= ti;
		}

		graphicUpdate(ellapsedTime);
	}
}

float BenchmarkerCore::ticksWanted() const{
	static const unsigned int FPS = 50;
	static const float TICKS = 1000.F / FPS;
	return TICKS;
}


ska::GameApp& ska::GameApp::get() {
	static BenchmarkerCore app;
	return app;
}
