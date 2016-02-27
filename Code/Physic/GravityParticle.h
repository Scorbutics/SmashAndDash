#ifndef DEF_GRAVITY_PARTICLE
#define DEF_GRAVITY_PARTICLE

#include "Particle.h"

class GravityParticle : public Particle
{

    public:
    GravityParticle(int idSprite, SDL_Rect pos, double lifetime, double splashTime, float acceleration, bool loop, bool relative);
    virtual void refresh();
    virtual void launch(SDL_Rect origin, float angle, unsigned int power);
    void addSlopeNoise(float x);
    void resetSlopeNoise();
    virtual ~GravityParticle();

    private:
    float m_ay, m_by, m_cy;  //coefficients de la parabole décrivant la chute libre
    //m_a : coefficient devant t² pour y, m_b : coefficient devant t pour y, m_c : constante pour y
    float m_ax, m_bx;

};

#endif
