#ifndef DEF_PHYSICSPRITE
#define DEF_PHYSICSPRITE

#include <SDL2/SDL.h>
#include "PhysicObject.h"
#include "../Graphic/Animation.h"
#include "../Graphic/Texture.h"

class WGameCore;

class PhysicSprite : public PhysicObject
{
	public:
		PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, SDL_Rect r, int offset[4]);
        PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, SDL_Rect r, int offset0, int offset1, int offset2, int offset3);
		PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, int x, int y, unsigned int w, unsigned int h, int offset0, int offset1, int offset2, int offset3);

		void display();
        void show(bool x);
        bool isVisible();
        void adaptPositionToBlock();

		Animation* getAnimation();
		Texture* getSprite();
        int getWidth();
        int getHeight();
        SDL_Rect getOffsetAndFrameSize();

        void setOffsetAndFrameSize(SDL_Rect sprite);
		void setID(int id);

		~PhysicSprite();
		void teleport(int x, int y);
		void displayShadow();

	protected:
	    bool m_visible;
		Texture m_sprite, m_shadow;
		Animation m_anim;

};

#endif
