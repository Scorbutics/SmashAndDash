/*#pragma once
#include "../../ska/Physic/GravityParticle.h"
#include "../../ska/Graphic/Rectangle.h"

class RainParticleManager;
class RainParticle :
	public ska::GravityParticle
{
public:
	RainParticle(RainParticleManager& parent, int id, int idSprite, ska::Rectangle pos, double lifetime, double splashTime, float acceleration, bool loop, bool relative);
	virtual ska::Point<int> getOrigin() const override;
	virtual void refresh() override;
	virtual ~RainParticle();

private:
	int m_id;
	RainParticleManager& m_parent;
};
*/
