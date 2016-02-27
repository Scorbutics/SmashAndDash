#pragma once
#include "GravityParticle.h"

class RainParticleManager;
class RainParticle :
	public GravityParticle
{
public:
	RainParticle(RainParticleManager& parent, int id, int idSprite, SDL_Rect pos, double lifetime, double splashTime, float acceleration, bool loop, bool relative);
	virtual SDL_Rect getOrigin() const override;
	virtual void refresh() override;
	virtual ~RainParticle();

private:
	int m_id;
	RainParticleManager& m_parent;
};

