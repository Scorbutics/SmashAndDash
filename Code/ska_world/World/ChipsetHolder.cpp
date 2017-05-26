
#include "Exceptions/CorruptedFileException.h"
#include "Exceptions/FileException.h"
#include "Draw/SDLSurface.h"
#include "Utils/StringUtils.h"
#include "Block.h"
#include "Utils/SkaConstants.h"
#include "Chipset.h"
#include "ChipsetHolder.h"
#include "Graphic/Animation.h"

ska::ChipsetHolder::ChipsetHolder(const std::string&) {
	SDLSurface fichierMCorr;
	const std::string& corrName = "." FILE_SEPARATOR "Chipsets" FILE_SEPARATOR "corr.png";
	m_corrFileWidth = 0;
	fichierMCorr.load32(corrName);

	if (fichierMCorr.getInstance() == nullptr) {
		throw FileException("Erreur lors du chargement de \"" + corrName + "\", fichier de correspondance entre tuiles du monde et position dans le chipset. "
			"Sans ce fichier, le chargement d'un chipset (et donc d'un monde) est impossible. " + std::string(SDL_GetError()));
	}

	m_corrFileWidth = fichierMCorr.getInstance()->w;
	buildCorrMap(fichierMCorr);
}

const std::string& ska::ChipsetHolder::getName() const {
	if (m_chipset == nullptr) {
		/* TODO exception */
	}

	return m_chipset->getName();
}

std::vector<ska::ScriptSleepComponent*> ska::ChipsetHolder::getScript(const std::string& id, const ScriptTriggerType& type, bool& autoBlackList) {
	return m_chipset->getScript(id, type, autoBlackList);
}

bool ska::ChipsetHolder::attach(const unsigned int blockSize, const std::string& chipsetName) {
	if (m_currentChipsetName != chipsetName) {
		m_currentChipsetName = chipsetName;
		m_chipset = ChipsetPtr(new Chipset(m_corr, m_corrFileWidth, blockSize, chipsetName));
		return true;
	}
	return false;
}

void ska::ChipsetHolder::update(BlockRenderable& block) {
	m_chipset->getRenderable().update(block);
}

void ska::ChipsetHolder::generateBlock(Color& key, Block** outputBlock, BlockRenderable** outputRenderable) {
	if (m_chipset == nullptr) {
		/* TODO exception */
	}

	return m_chipset->generateBlock(key, outputBlock, outputRenderable);
}

void ska::ChipsetHolder::render(ska::Point<int> pos, const BlockRenderable& block) const {
	m_chipset->getRenderable().render(pos, block);
}

void ska::ChipsetHolder::buildCorrMap(const SDLSurface& fichierMCorr) {
	for (int x = 0; x < fichierMCorr.getInstance()->w; x++) {
		for (int y = 0; y < fichierMCorr.getInstance()->h; y++) {
			Color c = fichierMCorr.getPixel32Color(x, y);
			if (m_corr.find(c) == m_corr.end()) {
				m_corr.insert(std::make_pair(c, Point<int>(x, y)));
			} else {
				Point<int>& pos2 = m_corr[c];
				throw CorruptedFileException("Chipset correspondance file (\"corr.png\") has several tiles with same color at ("
					+ StringUtils::intToStr(pos2.x) + "; " + StringUtils::intToStr(pos2.y) + ") and (" + StringUtils::intToStr(x) + "; " + StringUtils::intToStr(y)
					+ "). Please keep only one color correspondance per tile");
			}
		}
	}
}
