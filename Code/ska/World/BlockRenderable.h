#pragma once

#include <vector>
#include <memory>
#include "../Graphic/Animation.h"
#include "../Graphic/Point.h"

namespace ska {

	class BlockRenderable {
	public:
		BlockRenderable(const unsigned int blockSize, ska::Point<int> posChipset, bool auto_animation);
		~BlockRenderable() = default;

		void refresh();
		ska::Rectangle determineFrame(ska::Rectangle pos, const ska::Rectangle* rectAnim = NULL) const;
		void setSpriteFrame(unsigned int x);

	private:
		ska::Rectangle m_spritePosInChipset;
		bool m_auto_animation;
		Animation m_anim;

	};

	typedef std::unique_ptr<BlockRenderable> BlockRenderablePtr;
}