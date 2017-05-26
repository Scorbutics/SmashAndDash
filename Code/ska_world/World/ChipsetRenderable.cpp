#include <utility>
#include "../World/BlockRenderable.h"
#include "Utils/StringUtils.h"
#include "ChipsetRenderable.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"


ska::ChipsetRenderable::ChipsetRenderable(const unsigned int corrFileSize, const int blockSize, const std::string& chipsetName) :
m_animBlocks(375, 4, true, 0, 0, blockSize, blockSize) {
	load(chipsetName);
	m_blocks.resize(corrFileSize);
}

void ska::ChipsetRenderable::render(ska::Point<int> pos, const BlockRenderable& block) const {
    auto clip = m_animBlocks.getOffsetAndFrameSize();
	Rectangle chipsetPartRender = block.determineFrame(pos, &clip);
	m_chipset.render(pos.x, pos.y, &chipsetPartRender);
}

void ska::ChipsetRenderable::update(BlockRenderable& block) {
	block.refresh();
	m_animBlocks.getRectOfCurrentFrame();
}

ska::BlockRenderablePtr& ska::ChipsetRenderable::generateBlock(const int id, const int blockSize, Point<int> posCorr, bool auto_anim) {
	if (m_blocks[id] == nullptr) {
		m_blocks[id] = std::move(BlockRenderablePtr(new BlockRenderable(blockSize, posCorr, auto_anim)));
	}
	return m_blocks[id];
}

void ska::ChipsetRenderable::load(const std::string& chipsetName) {
	m_chipset.load(chipsetName);
}


const ska::Texture& ska::ChipsetRenderable::getTexture() const{
	return m_chipset;
}
