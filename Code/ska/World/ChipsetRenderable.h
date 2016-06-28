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
		ChipsetRenderable(const int blockSize, const std::string& chipsetName);
		void render(Rectangle pos, const BlockRenderablePtr& block);
		const Texture& getTexture() const;
		~ChipsetRenderable() = default;

	private:
		void load(const std::string& chipsetName);

		Texture m_chipset;
		Animation m_animBlocks;
	};
}