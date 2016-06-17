#ifndef DEF_BLOCK
#define DEF_BLOCK

#include <vector>
#include <memory>
#include "../Graphic/Animation.h"
#include "../Graphic/Point.h"

namespace ska {
	class Block
	{
	public:
		Block(const unsigned int blockSize, int corrFileWidth, ska::Point<int> posChipset, unsigned int properties, bool auto_animation, int col);
		unsigned int getProperties();
		int getCollision();
		unsigned int getID() const;
		ska::Rectangle refresh(ska::Rectangle pos, const ska::Rectangle* rectAnim = NULL);
		~Block();
		void setSpriteFrame(unsigned int x);

	private:
		unsigned int m_properties;
		unsigned int m_id;
		ska::Rectangle m_spritePosInChipset;
		bool m_auto_animation;
		int m_collision;
		Animation m_anim;

	};

	typedef std::unique_ptr<Block> BlockPtr;
}
#endif
