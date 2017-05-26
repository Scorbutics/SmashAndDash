#pragma once

#include <vector>
#include <memory>
#include "Graphic/Animation.h"
#include "Point.h"

namespace ska {

	class BlockRenderable {
	public:
		BlockRenderable(const unsigned int blockSize, Point<int> posChipset, bool auto_animation);
		~BlockRenderable() = default;

		void refresh();
		Rectangle determineFrame(ska::Point<int>, const Rectangle* rectAnim = NULL) const;
		void setSpriteFrame(unsigned int x);

	private:
		Rectangle m_spritePosInChipset;
		bool m_auto_animation;
		Animation m_anim;

	};

	typedef std::unique_ptr<BlockRenderable> BlockRenderablePtr;
}
