#include <fstream>
#include "Exceptions/CorruptedFileException.h"
#include "Exceptions/FileException.h"
#include "../World/Block.h"
#include "../World/BlockRenderable.h"
#include "Utils/SkaConstants.h"
#include "Utils/StringUtils.h"
#include "Chipset.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"

ska::Chipset::Chipset(const std::unordered_map<Color, Point<int>>& corr, const unsigned int corrFileWidth, const int blockSize, const std::string& chipsetName) :
	m_corr(corr),
	m_blockSize(blockSize),
	m_corrFileWidth(corrFileWidth),
	m_chipsetName(chipsetName),
	m_renderable(static_cast<const unsigned int>(corr.size()), blockSize, chipsetName) {
	load();
	m_blocks.resize(corr.size());
}

void ska::Chipset::load() {
	m_sChipset.load32(m_chipsetName);
	if (m_sChipset.getInstance() == nullptr) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + m_chipsetName + "\", fichier du chipset. " + std::string(SDL_GetError()));
	}

	m_sCol.load32(m_chipsetName + ".col");
	if (m_sCol.getInstance() == nullptr) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + m_chipsetName + ".col\", fichier de collsions du chipset. " + std::string(SDL_GetError()));
	}

	m_sProperties.load32(m_chipsetName + ".prop");
	if (m_sProperties.getInstance() == nullptr) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + m_chipsetName + ".prop\", fichier de propriétés du chipset. " + std::string(SDL_GetError()));
	}

	m_darkColor = SDL_MapRGB(m_sChipset.getFormat(), 70, 70, 70);
	m_lightColor = SDL_MapRGB(m_sChipset.getFormat(), 170, 170, 170);
	m_whiteColor = SDL_MapRGB(m_sChipset.getFormat(), 255, 255, 255);

    const std::string& chipsetFolder = m_chipsetName.substr(0, m_chipsetName.find_last_of('.'));
	std::ifstream scriptList((chipsetFolder + "" FILE_SEPARATOR "scripts.txt").c_str(), std::ifstream::in);
	std::string ss;

	if (scriptList.fail()) {
		throw FileException("Erreur lors de l'ouverture du fichier \"" + chipsetFolder + "" FILE_SEPARATOR "scripts.txt" + "\", fichier de scripts du chipset. " + std::string(SDL_GetError()));
	}

	while (getline(scriptList, ss)) {
		fillScript(chipsetFolder, ss, EnumScriptTriggerType::TOUCH);
		fillScript(chipsetFolder, ss, EnumScriptTriggerType::MOVE_OUT);
		fillScript(chipsetFolder, ss, EnumScriptTriggerType::MOVE_IN);
		fillScript(chipsetFolder, ss, EnumScriptTriggerType::ACTION);
		fillScript(chipsetFolder, ss, EnumScriptTriggerType::AUTO);
	}

}

void ska::Chipset::fillScript(const std::string& chipsetFolder, const std::string& id, const ScriptTriggerType& type) {
	std::ifstream currentScript;
	const std::string fullName = chipsetFolder + "" FILE_SEPARATOR "Scripts" FILE_SEPARATOR "" + id + "_" + (char)(type + '0') + ".txt";
	currentScript.open(fullName, std::ios_base::in);
	if (currentScript.fail()) {
		return;
	}

	ScriptSleepComponent ssc;
	ssc.triggeringType = type;
	ssc.name = fullName;
	ssc.period = 1000;

	if (type == EnumScriptTriggerType::AUTO) {
		m_autoScripts.insert(std::make_pair(StringUtils::strToInt(id), ssc));
	} else {
		m_triggeredScripts.insert(make_pair(fullName, ssc));
	}

}

std::vector<ska::ScriptSleepComponent*> ska::Chipset::getScript(const std::string& id, const ScriptTriggerType& reason, bool& autoBlackList) {
	std::vector<ScriptSleepComponent*> result;

	if (reason == EnumScriptTriggerType::AUTO && !autoBlackList) {
		for (auto& s : m_autoScripts) {
			result.push_back(&s.second);
		}
		autoBlackList = true;
	} else if(!id.empty()) {
		const std::string fullId = id + "_" + (char)(reason + '0');
		const std::string fullName = m_chipsetName.substr(0, m_chipsetName.find_last_of('.')) + "" FILE_SEPARATOR "Scripts" FILE_SEPARATOR "" + fullId + ".txt";
		if (m_triggeredScripts.find(fullName) != m_triggeredScripts.end()) {
			result.push_back(&m_triggeredScripts.at(fullName));
		}
	}

	return result;
}

const std::string& ska::Chipset::getName() const {
	return m_chipsetName;
}

ska::ChipsetRenderable& ska::Chipset::getRenderable() {
	return m_renderable;
}

void ska::Chipset::generateBlock(Color& key, Block** outputBlock, BlockRenderable** outputRenderable) {
	if (key.r != 255 || key.g != 255 || key.b != 255) {
		if (m_corr.find(key) != m_corr.end()) {
			Point<int> posCorr = m_corr.at(key);
			Uint8 prop = m_sProperties.getPixel32Color(posCorr.x, posCorr.y).r;
			Uint32 col = m_sCol.getPixel32(posCorr.x, posCorr.y);

			int collision = (col == m_whiteColor || col == m_lightColor) ? BLOCK_COL_NO : BLOCK_COL_YES;
			bool auto_anim = (col == m_darkColor || col == m_lightColor);

			const int id = posCorr.x + posCorr.y * m_corrFileWidth;
			if (m_blocks[id] == nullptr) {
				m_blocks[id] = std::move(BlockPtr(new Block(m_corrFileWidth, posCorr, prop, collision)));
			}

			*outputBlock = m_blocks[id].get();
			*outputRenderable = m_renderable.generateBlock(id, m_blockSize, posCorr, auto_anim).get();
		}
		else {
			throw CorruptedFileException("Impossible de trouver la correspondance en pixel (fichier niveau corrompu)");
		}
	}
	else {
		*outputBlock = nullptr;
		*outputRenderable = nullptr;
	}
}
