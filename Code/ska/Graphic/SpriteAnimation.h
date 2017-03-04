#ifndef DEF_SPRITEANIMATION
#define DEF_SPRITEANIMATION

#include "Animation.h"
#include "Texture.h"
#include "Point.h"
#include "Rectangle.h"
#include <memory>

namespace ska {
	class SpriteAnimation
	{
	public:
		SpriteAnimation(int type, int id, unsigned int framesNumber = 4, unsigned int delay = 300, int alpha = -1);
		SpriteAnimation(int type, int id, Point<int> pos, unsigned int framesNumber = 4, unsigned int delay = 300, int alpha = -1);
		void refresh();

		void setPos(Point<int> pos);
		void setID(int id);
		void setType(int type);

		Animation* getAnimation();
		unsigned int getHeight();
		unsigned int getWidth();

		void display() const;

	private:
		Animation m_anim;
		Texture m_sprite;
		int m_type, m_id;
		Point<int> m_pos;
		Rectangle m_offset;
	};
	typedef std::unique_ptr<SpriteAnimation> SpriteAnimationPtr;
}
#endif
