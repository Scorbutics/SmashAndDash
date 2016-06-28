#include <fstream>
#include "../Exceptions/CorruptedFileException.h"
#include "../Exceptions/FileException.h"
#include "../World/Block.h"
#include "../World/BlockRenderable.h"
#include "../Utils/SkaConstants.h"
#include "../Utils/StringUtils.h"
#include "ChipsetRenderable.h"
#include "../Script/ScriptSleepComponent.h"

ska::ChipsetRenderable::ChipsetRenderable(const int blockSize, const std::string& chipsetName) :
m_animBlocks(375, 4, true, 0, 0, blockSize, blockSize) {
	load(chipsetName);
}

void ska::ChipsetRenderable::render(Rectangle pos, const BlockRenderablePtr& block) {
	Rectangle& chipsetPartRender = block->refresh(pos, &m_animBlocks.getRectOfCurrentFrame());
	m_chipset.render(pos.x, pos.y, &chipsetPartRender);
}

void ska::ChipsetRenderable::load(const std::string& chipsetName) {
	m_chipset.load(chipsetName);
}


const ska::Texture& ska::ChipsetRenderable::getTexture() const{
	return m_chipset;
}
