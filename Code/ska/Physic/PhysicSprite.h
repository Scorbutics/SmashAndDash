#ifndef DEF_PHYSICSPRITE
#define DEF_PHYSICSPRITE

#include "PhysicObject.h"
#include "../Graphic/Animation.h"
#include "../Graphic/Texture.h"

class WGameCore;
namespace ska {
	class PhysicSprite : public PhysicObject
	{
	public:
		PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, Rectangle r, int offset[4]);
		PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, Rectangle r, int offset0, int offset1, int offset2, int offset3);
		PhysicSprite(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, int x, int y, unsigned int w, unsigned int h, int offset0, int offset1, int offset2, int offset3);

		void display();
		void show(bool x);
		bool isVisible();
		void adaptPositionToBlock();

		Animation* getAnimation();
		Texture* getSprite();
		int getWidth();
		int getHeight();
		Rectangle getOffsetAndFrameSize();

		void setOffsetAndFrameSize(Rectangle sprite);
		void setID(int id);

		~PhysicSprite();
		void teleport(int x, int y);
		void displayShadow();

	protected:
		bool m_visible;
		Animation m_anim;
		Texture m_shadow;
		Texture m_sprite;

	};
}
#endif
