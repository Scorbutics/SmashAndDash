#include <fstream>
#include "../Exceptions/CorruptedFileException.h"
#include "../Exceptions/FileException.h"
#include "../World/Block.h"
#include "../World/BlockRenderable.h"
#include "../Utils/SkaConstants.h"
#include "../Utils/StringUtils.h"
#include "ChipsetRenderable.h"
#include "../Script/ScriptSleepComponent.h"


ska::ChipsetRenderable::ChipsetRenderable(const unsigned int corrFileSize, const int blockSize, const std::string& chipsetName) :
m_animBlocks(375, 4, true, 0, 0, blockSize, blockSize) {
	load(chipsetName);
	m_blocks.resize(corrFileSize);
}

void ska::ChipsetRenderable::render(Rectangle pos, const std::shared_ptr<BlockRenderable>& block) {
	Rectangle& chipsetPartRender = block->refresh(pos, &m_animBlocks.getRectOfCurrentFrame());
	m_chipset.render(pos.x, pos.y, &chipsetPartRender);
}

std::shared_ptr<ska::BlockRenderable> ska::ChipsetRenderable::generateBlock(const int id, const int blockSize, ska::Point<int> posCorr, bool auto_anim) {
	if (m_blocks[id] == nullptr) {
		m_blocks[id] = std::move(std::shared_ptr<ska::BlockRenderable>(new BlockRenderable(blockSize, posCorr, auto_anim)));
	}
	return m_blocks[id];
}

void ska::ChipsetRenderable::load(const std::string& chipsetName) {
	m_chipset.load(chipsetName);
}


const ska::Texture& ska::ChipsetRenderable::getTexture() const{
	return m_chipset;
}
