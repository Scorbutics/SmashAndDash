#pragma once
#include <unordered_map>
#include <memory>
#include "Graphic/Texture.h"
#include "Graphic/Animation.h"
#include "../World/BlockRenderable.h"

namespace ska {

	class ChipsetRenderable {
	public:
		ChipsetRenderable(const unsigned int corrFileSize, const int blockSize, const std::string& chipsetName);
		void render(ska::Point<int> pos, const BlockRenderable& block) const;
		void update(BlockRenderable& block);
		BlockRenderablePtr& generateBlock(const int id, const int blockSize, Point<int> posCorr, bool auto_anim);
		const Texture& getTexture() const;
		~ChipsetRenderable() = default;

	private:
		std::vector<BlockRenderablePtr> m_blocks;
		void load(const std::string& chipsetName);

		Texture m_chipset;
		Animation m_animBlocks;
	};
}
