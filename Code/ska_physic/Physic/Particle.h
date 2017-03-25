#pragma once

#include <memory>
#include <iostream>
#include <vector>
#include "Point.h"

namespace ska {
	class Particle {

	public:
		Particle(int idSprite, unsigned int weight, Rectangle pos, double lifetime, double splashTime, bool loop, bool relative);
		virtual void refresh() = 0;
		//void display();

		virtual void launch(Point<int> origin, float angle, unsigned int power);
		virtual void addSlopeNoise(float x) = 0;
		virtual void resetSlopeNoise() = 0;
		virtual Point<int> getOrigin() const;

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
		Rectangle getPos();
		std::vector<Rectangle> collisionNPC();
		void setPos(int x, int y);
		void toSkillParticle();
		void setNoise(int x);
		virtual ~Particle();
		void setLoop(bool loop);

	protected:
		//Texture m_sprite;
		unsigned int m_weight, m_power, m_countWind;
		Rectangle m_pos, m_spriteSize;
		//Animation m_anim;
		float m_angle, m_slopeNoise;
		bool m_active, m_loop, m_relative, m_skill;
		int m_idSprite, m_type, m_state, m_noise;
		double m_t, m_lifetime, m_splashTime; //m_splashTime : temps pendant lequel la particule affiche son sprite de fin de vie (mettre à 0 s'il n'y a pas de sprite de fin de vie)

	private:
		Point<int> m_fixedOrigin;

	};

	typedef std::unique_ptr<Particle> ParticlePtr;
}

