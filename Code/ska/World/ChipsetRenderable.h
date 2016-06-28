#pragma once
#include <unordered_map>
#include <memory>
#include "../Graphic/Texture.h"
#include "../Graphic/Animation.h"

namespace ska {
	class BlockRenderable;
	typedef std::unique_ptr<BlockRenderable> BlockRenderablePtr;

	class ChipsetRenderable {
	public:
		ChipsetRenderable(const unsigned int corrFileSize, const int blockSize, const std::string& chipsetName);
		void render(Rectangle pos, const std::shared_ptr<BlockRenderable>& block);
		std::shared_ptr<ska::BlockRenderable> generateBlock(const int id, const int blockSize, ska::Point<int> posCorr, bool auto_anim);
		const Texture& getTexture() const;
		~ChipsetRenderable() = default;

	private:
		std::vector<std::shared_ptr<BlockRenderable>> m_blocks;
		void load(const std::string& chipsetName);

		Texture m_chipset;
		Animation m_animBlocks;
	};
}