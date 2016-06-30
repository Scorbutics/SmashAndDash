#include <cstdint>

#include "World.h"
#include "Layer.h"
#include "Block.h"
#include "../Utils/StringUtils.h"
#include "../Utils/RectangleUtils.h"
#include "../Physic/PhysicObject.h"
#include "LayerE.h"
#include "../Script/ScriptSleepComponent.h"
#include "../Graphic/System/CameraSystem.h"

ska::World::World(const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight) :  
	m_blockSize(tailleBloc), 
	m_chipset("."FILE_SEPARATOR"Chipsets"FILE_SEPARATOR"corr.png"),
	m_autoScriptsPlayed(false), 
	m_lTop(*this, &m_lMid),
	m_lMid(*this, &m_lBot), 
	m_lBot(*this), 
	m_layerE(*this) {
}

void ska::World::linkCamera(CameraSystem* cs) {
	m_cameraSystem = cs;
	if (m_cameraSystem != NULL) {
		m_cameraSystem->worldResized(getPixelWidth(), getPixelHeight());
	}
}

ska::ChipsetHolder& ska::World::getChipset() {
	return m_chipset;
}

std::string ska::World::getFileName() const {
	return m_fileName;
}

bool ska::World::isBlockDodgeable(const int i, const int j) {
	Block* b = m_lMid.getBlock(i, j);
	return (b != nullptr && b->getProperties() == BLOCK_PROP_JUMPWALL);
}

bool ska::World::getCollision(const int i, const int j) {
	if (m_lBot.getBlockCollision(i, j) == BLOCK_COL_YES &&
		(m_lMid.getBlockCollision(i, j) != BLOCK_COL_NO)) {
		return true;
	}

	if (m_lMid.getBlockCollision(i, j) == BLOCK_COL_YES) {
		return true;
	}

    return false;
}

void ska::World::update() {
	m_lBot.getRenderable().update();
	m_lMid.getRenderable().update();
	m_lTop.getRenderable().update();
}

bool ska::World::canMoveToPos(ska::Rectangle hitbox) {
	ska::Point<int> chd, chg, cbg;


	//position coin haut droit hitbox
	chd.x = hitbox.x + hitbox.w;
	chd.y = hitbox.y ;

	//position coin haut gauche hitbox
	chg.x = hitbox.x;
	chg.y = hitbox.y;

	//position coin bas gauche hitbox
	cbg.x = hitbox.x;
	cbg.y = hitbox.y + hitbox.h;

	const int yLimit = cbg.y / m_blockSize;
	const int xLimit = chd.x / m_blockSize;

	for (int y = chg.y / m_blockSize; y <= yLimit; y++) {
		for (int x = chg.x / m_blockSize; x <= xLimit; x++) {
			if (getCollision(x, y) /*&& !isBlockDodgeable(i, j)*/) {
				return false;
			}
		}
	}
	return true;
}

const ska::Rectangle* ska::World::getView() const {
	return m_cameraSystem == NULL ? NULL : m_cameraSystem->getDisplay();
}

ska::LayerRenderable& ska::World::getLayerRenderable(int level) {
	switch (level) {
	case 0:
		return m_lBot.getRenderable();
		break;
	case 1:
		return m_lMid.getRenderable();
		break;
	default:
		return m_lTop.getRenderable();
		break;
	}
}

ska::LayerE& ska::World::getLayerEvent() {
	return m_layerE;
}

std::string ska::World::getName() const {
    return m_worldName;
}

std::string ska::World::getGenericName() const {
    return m_genericName;
}

void ska::World::load(const std::string& fileName, const std::string& chipsetName) {
	bool worldChanged = fileName != m_fileName;
	m_autoScriptsPlayed = false;
	bool chipsetChanged = m_chipset.attach(m_blockSize, chipsetName);

	if (worldChanged) {
		m_genericName = fileName.substr(0, fileName.find_last_of('.'));
		m_worldName = m_genericName.substr(m_genericName.find_last_of('/') + 1, m_genericName.size());
		m_fileName = fileName;
		
		getData();
	}

	if (worldChanged || chipsetChanged) {
		const std::string& botLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"" + m_genericName + ".bmp";
		const std::string& midLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"" + m_genericName + "M.bmp";
		const std::string& topLayerName = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"" + m_genericName + "T.bmp";
		const std::string& eventLayerName = m_genericName + "E.txt";

		m_lBot.clear();
		m_lMid.clear();
		m_lTop.clear();

		m_lBot.reset(botLayerName, chipsetName);
		m_lMid.reset(midLayerName, chipsetName);
		m_lTop.reset(topLayerName, chipsetName);

		m_layerE.changeLevel(eventLayerName);
		
		if (m_cameraSystem != NULL) {
			m_cameraSystem->worldResized(getPixelWidth(), getPixelHeight());
		}
	}
		
}

std::vector<ska::ScriptSleepComponent*> ska::World::chipsetScript(const ska::Point<int>& posToLookAt, const ScriptTriggerType& reason) {
	std::vector<ska::ScriptSleepComponent*> result;
	
	if (reason == EnumScriptTriggerType::AUTO) {
		std::vector<ska::ScriptSleepComponent*> tmp = m_chipset.getScript("", reason, m_autoScriptsPlayed);
		for (auto& ssc : tmp) {
			if (ssc != nullptr) {
				ssc->context = getName();
				result.push_back(ssc);
			}
		}
		return result;
	}
	

	/* TODO autres layers ??? */
	Block* b = m_lBot.getBlock(posToLookAt.x / m_blockSize, posToLookAt.y / m_blockSize);
	if (b != nullptr) {
		const unsigned int id = b->getID();
		std::vector<ska::ScriptSleepComponent*> tmp = m_chipset.getScript(ska::StringUtils::intToStr(id), reason, m_autoScriptsPlayed);
		for (auto& ssc : tmp) {
			if (ssc != nullptr) {
				ssc->context = getName();
				result.push_back(ssc);
			}
		}
	}
	return result;
	
}

ska::Block* ska::World::getHigherBlock(const unsigned int i, const unsigned int j) {
	Block* bBot = m_lBot.getBlock(i, j);
	Block* bMid = m_lMid.getBlock(i, j);
	Block* bTop = m_lTop.getBlock(i, j);

	if (bTop != nullptr) {
		return bTop;
	}

	if (bMid != nullptr) {
		return bMid;
	}

	return bBot;
}


int ska::World::getNbrBlocX() const {
    return m_nbrBlockX;
}

unsigned int ska::World::getPixelWidth() const {
	return m_nbrBlockX*m_blockSize;
}

int ska::World::getNbrBlocY() const {
	return m_nbrBlockY;
}

unsigned int ska::World::getPixelHeight() const {
	return m_nbrBlockY*m_blockSize;
}

const unsigned int ska::World::getBlockSize() const {
	return m_blockSize;
}

void ska::World::setNbrBlocX(int nbrBlockX) {
	m_nbrBlockX = nbrBlockX;
}

void ska::World::setNbrBlocY(int nbrBlockY) {
	m_nbrBlockY = nbrBlockY;
}



void ska::World::getRainFromData(std::string stringDataFile) {
    int idsprite, acceleration, density;	
	IniReader reader(stringDataFile);

    if(reader.getString("Rain id_sprite") != "STRINGNOTFOUND") {
		idsprite = reader.getInt("Rain id_sprite");
		acceleration = reader.getInt("Rain acceleration");
		density = reader.getInt("Rain density");

    } else {
        std::clog << "La pluie est inexistante sur cette map" << std::endl;
    }
}

void ska::World::getMobSettingsFromData() {
	m_mobSettings.clear();

	unsigned int i = 0;
	do  {
		m_mobSettings.push_back(IniReader( "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"Monsters"FILE_SEPARATOR"" + ska::StringUtils::intToStr(i) + ".ini"));
		i++;
	} while(m_mobSettings[i-1].isLoaded());

	//le dernier élément est invalide, on le supprime donc
	m_mobSettings.pop_back();

}

void ska::World::getData() {
    std::string stringDataFile = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"" + m_genericName + ".ini";

    getRainFromData(stringDataFile);
	getMobSettingsFromData();
}

std::vector<ska::IniReader>& ska::World::getMobSettings() {
	return m_mobSettings;
}

