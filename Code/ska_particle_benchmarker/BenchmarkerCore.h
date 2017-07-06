#pragma once
#include "../ska_core/GameApp.h"
#include "../ska_particle/ParticleSystem.h"
#include "../ska_core/Core/Window.h"
#include "Inputs/RawInputListener.h"
#include "FpsCalculator.h"
#include "Impl/SphereAttractorParticleUpdater.h"

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
	ska::RawInputListener m_inputListener;
	ska::Window m_window;
	ska::ParticleSystem m_particles;
	ska::SphereAttractorParticleUpdater* m_attractor;
	ska::FpsCalculator m_fpsCalculator;
};

