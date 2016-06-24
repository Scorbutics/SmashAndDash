
#include "../Exceptions/CorruptedFileException.h"
#include "../Exceptions/FileException.h"
#include "../Graphic/SDLSurface.h"
#include "../Utils/StringUtils.h"
#include "Block.h"
#include "../Utils/SkaConstants.h"
#include "Chipset.h"
#include "ChipsetHolder.h"


ska::ChipsetHolder::ChipsetHolder(const std::string& correspondanceFile) {
	ska::SDLSurface fichierMCorr;
	const std::string& corrName = "."FILE_SEPARATOR"Chipsets"FILE_SEPARATOR"corr.png";
	m_corrFileWidth = 0;
	fichierMCorr.load32(corrName);
	
	if (fichierMCorr.getInstance() == nullptr) {
		throw ska::FileException("Erreur lors du chargement de \"" + corrName + "\", fichier de correspondance entre tuiles du monde et position dans le chipset. "
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

std::vector<ska::ScriptSleepComponent*> ska::ChipsetHolder::getScript(const std::string& id, const ska::ScriptTriggerType& type, bool& autoBlackList) {
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

ska::BlockPtr ska::ChipsetHolder::generateBlock(ska::Color& key) {
	if (m_chipset == nullptr) {
		/* TODO exception */
	}

	return m_chipset->generateBlock(key);
}

ska::Texture& ska::ChipsetHolder::getChipset() {
	return m_chipset->getTexture();
}

void ska::ChipsetHolder::buildCorrMap(const ska::SDLSurface& fichierMCorr) {
	for (int x = 0; x < fichierMCorr.getInstance()->w; x++) {
		for (int y = 0; y < fichierMCorr.getInstance()->h; y++) {
			ska::Color c = fichierMCorr.getPixel32Color(x, y);
			if (m_corr.find(c) == m_corr.end()) {
				m_corr.insert(std::make_pair(c, ska::Point<int>(x, y)));
			} else {
				ska::Point<int>& pos2 = m_corr[c];
				throw ska::CorruptedFileException("Chipset correspondance file (\"corr.png\") has several tiles with same color at ("
					+ ska::StringUtils::intToStr(pos2.x) + "; " + ska::StringUtils::intToStr(pos2.y) + ") and (" + ska::StringUtils::intToStr(x) + "; " + ska::StringUtils::intToStr(y)
					+ "). Please keep only one color correspondance per tile");
			}
		}
	}
}