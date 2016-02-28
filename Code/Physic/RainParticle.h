#pragma once
#include "GravityParticle.h"
#include "../Graphic/Rectangle.h"

class RainParticleManager;
class RainParticle :
	public GravityParticle
{
public:
	RainParticle(RainParticleManager& parent, int id, int idSprite, ska::Rectangle pos, double lifetime, double splashTime, float acceleration, bool loop, bool relative);
	virtual ska::Rectangle getOrigin() const override;
	virtual void refresh() override;
	virtual ~RainParticle();

private:
	int m_id;
	RainParticleManager& m_parent;
};

