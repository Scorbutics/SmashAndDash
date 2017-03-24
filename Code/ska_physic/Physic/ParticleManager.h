#ifndef DEF_PARTICLE_MANAGER
#define DEF_PARTICLE_MANAGER

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "Particle.h"
#include "../Graphic/Texture.h"
#include "../Graphic/Draw/DrawableFixedPriority.h"
#include "../Graphic/Rectangle.h"

namespace ska {
	class ParticleManager : public DrawableFixedPriority
	{

	public:
		ParticleManager();
		virtual ~ParticleManager();

		void playCrumbling(int idSprite, float acceleration, float density, unsigned int duration);
		void playEffect(int idSprite, float acceleration, float density, unsigned int duration, Rectangle rect);
		void removeAll();
		void stop();
		void hide(bool h);

		virtual bool isVisible() const override;
		virtual void display() const override;

		virtual void refresh();
		int collisionNPC();



	protected:
		void remove(unsigned int index);
		void launch(float angle, unsigned int power);
		/* TODO : GravityParticle, CircleParticle, LinearParticle en tant que classes... */
		//void addGravityParticle(int idSprite, Rectangle pos, double lifetime, double splashTime, float acceleration, bool loop, bool relative);
		void addParticle(ParticlePtr& particle);
		void addCircleParticle(int idSprite, unsigned int weight);
		void addLinearParticle(int idSprite, unsigned int weight);

		std::vector<std::unique_ptr<Particle>> m_particles;
		Texture m_rainWeather;
		int m_sens;
		bool m_active;
		unsigned int m_duration, m_t0;
	};
}
#endif
