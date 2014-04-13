#ifndef DEF_LINEARPARTICLE
#define DEF_LINEARPARTICLE

#include "Particle.h"

class LinearParticle: public Particle
{
	public:
	LinearParticle(int idSprite, SDL_Rect pos, double lifetime, double splashTime, bool loop, bool relative);
	virtual void refresh(int wind);
	virtual void addSlopeNoise(float x);
	virtual void resetSlopeNoise();
	virtual void launch(SDL_Rect origin, float angle, unsigned int power);
	~LinearParticle();


	private:
	float m_ay, m_by;  //coefficients de la droite selon y
	float m_ax, m_bx;  //coefficients de la droite selon x

};

#endif
