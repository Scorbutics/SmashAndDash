#pragma once
#include "../ska_core/GameApp.h"
#include "../ska_particle/ParticleEffect.h"
#include "../ska_core/Core/Window.h"
#include "Inputs/RawInputListener.h"
#include "FpsCalculator.h"
#include "Impl/EulerAttractorParticleUpdater.h"
#include "ParticleSystem.h"

class BenchmarkerCore : 
	public ska::GameApp {

public:
	BenchmarkerCore();
	virtual ~BenchmarkerCore() = default;
	int onTerminate(ska::TerminateProcessException&) override;
	int onException(ska::GenericException&) override;
	void eventUpdate(const float ti);
	void graphicUpdate(const unsigned long ellapsed_time) const;
	void run() override;
	float ticksWanted() const override;

private:
	void buildFireworks();

	ska::RawInputListener m_inputListener;
	ska::Window m_window;
	ska::ParticleEffect m_particles;
	ska::ParticleSystem m_particleSystem;
	ska::EulerAttractorParticleUpdater* m_attractor;
	ska::FpsCalculator m_fpsCalculator;
};

