#ifndef DEF_PARTICLE
#define DEF_PARTICLE


#include <iostream>
#include <vector>
#include "../Graphic/Texture.h"
#include "../Graphic/Animation.h"
#include "../Graphic/Point.h"

namespace ska {
	class Particle
	{

	public:
		Particle(int idSprite, unsigned int weight, ska::Rectangle pos, double lifetime, double splashTime, bool loop, bool relative);
		virtual void refresh() = 0;
		void display();
		virtual void launch(ska::Point<int> origin, float angle, unsigned int power);
		virtual void addSlopeNoise(float x) = 0;
		virtual void resetSlopeNoise() = 0;
		virtual ska::Point<int> getOrigin() const;

		void active();
		void destroy();
		bool loop();
		unsigned int getElapsedTime();
		void setElapsedTime(unsigned int t);
		bool isActive();
		bool isRelative();
		unsigned int getPower();
		float getAngle();
		int getState();
		ska::Rectangle getPos();
		std::vector<ska::Rectangle> collisionNPC();
		void setPos(int x, int y);
		void toSkillParticle();
		void setNoise(int x);
		virtual ~Particle();
		void setLoop(bool loop);

	protected:
		Texture m_sprite;
		unsigned int m_weight, m_power, m_countWind;
		ska::Rectangle m_pos, m_spriteSize;
		Animation m_anim;
		float m_angle, m_slopeNoise;
		bool m_active, m_loop, m_relative, m_skill;
		int m_idSprite, m_type, m_state, m_noise;
		double m_t, m_lifetime, m_splashTime; //m_splashTime : temps pendant lequel la particule affiche son sprite de fin de vie (mettre à 0 s'il n'y a pas de sprite de fin de vie)

	private:
		ska::Point<int> m_fixedOrigin;

	};

	typedef std::unique_ptr<Particle> ParticlePtr;
}
#endif
