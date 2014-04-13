#ifndef DEF_FORCE
#define DEF_FORCE

#include <SDL2/SDL.h>

class Force
{
	public:
	    Force();
		Force(SDL_Rect pos, float power, float angle);

		SDL_Rect getPos();
		float getPower();
		float getAngle();

		void setPos(SDL_Rect pos);
		void setPower(float power);
		void setAngle(float angle);

		~Force();

	protected:
		SDL_Rect m_pos;
		float m_angle, m_power;

};

#endif
