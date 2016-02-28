#pragma once
#include "../ska/Physic/ParticleManager.h"

class RainParticleManager : public ParticleManager
{
public:
	RainParticleManager();

	int getWind() const;
	virtual void refresh() override;
	
	virtual void display() override;
	void displayRainFog();
	void playRain(int idSprite, float acceleration, float density, unsigned int duration);

	virtual ~RainParticleManager();

private:
	unsigned int m_wind, m_countWind;
	double m_Twind;
};

