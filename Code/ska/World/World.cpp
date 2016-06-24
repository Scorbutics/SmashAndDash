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

using namespace std;

ska::World::World(const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight) :  
	m_blockSize(tailleBloc),  
	m_animBlocks(375, 4, true, 0, 0, tailleBloc, tailleBloc), 
	m_chipset("."FILE_SEPARATOR"Chipsets"FILE_SEPARATOR"corr.png"),
	m_autoScriptsPlayed(false) {
}

void ska::World::linkCamera(CameraSystem* cs) {
	m_cameraSystem = cs;
	if (m_cameraSystem != NULL) {
		m_cameraSystem->worldResized(getPixelWidth(), getPixelHeight());
	}
}

void ska::World::load(string fileName, string chipsetName) {
	changeLevel(fileName, chipsetName);
}

ska::ChipsetHolder& ska::World::getChipset() {
	return m_chipset;
}

std::string ska::World::getFileName() {
	return m_fileName;
}


void ska::World::setWind(int wind) {
	m_windDirection = wind;
}

int ska::World::getWind() const {
	return m_windDirection;
}

ska::Animation& ska::World::getChipsetAnimation() {
	return m_animBlocks;
}

bool ska::World::isBlockDodgeable(const int i, const int j) {
	return (m_lMid->getBlock(i, j)->getProperties() == BLOCK_PROP_JUMPWALL);
}

bool ska::World::getCollision(const int i, const int j) {
	if (m_lBot == NULL) {
		return false;
	}

	if (m_lBot->getBlockCollision(i, j) == BLOCK_COL_YES &&
		(m_lMid->getBlockCollision(i, j) != BLOCK_COL_NO)) {
		return true;
	}

	if (m_lMid->getBlockCollision(i, j) == BLOCK_COL_YES) {
		return true;
	}

    return false;
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

ska::LayerPtr& ska::World::getLayerBot() {
	return m_lBot;
}

ska::LayerPtr& ska::World::getLayerMid() {
	return m_lMid;
}

ska::LayerPtr& ska::World::getLayerTop() {
	return m_lTop;
}

ska::LayerEPtr& ska::World::getLayerEvent() {
	return layerE;
}

string ska::World::getName() {
    return m_worldName;
}

string ska::World::getGenericName() {
    return m_genericName;
}

const std::string& ska::World::getChipsetName() const {
	return m_chipset.getName();
}

void ska::World::changeLevel(string fileName, string chipsetName) {
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

		if (m_lBot != nullptr) {
			m_lBot->clear();
			m_lBot->reset(botLayerName, chipsetName);
		} else {
			m_lBot = LayerPtr(new Layer(*this, botLayerName, chipsetName));
		}

		if (m_lMid != nullptr) {
			m_lMid->clear();
			m_lMid->reset(midLayerName, chipsetName);
		} else {
			m_lMid = LayerPtr(new Layer(*this, midLayerName, chipsetName, m_lBot.get()));
		}

		if (m_lTop != nullptr) {
			m_lTop->clear();
			m_lTop->reset(topLayerName, chipsetName);
		} else {
			m_lTop = LayerPtr(new Layer(*this, topLayerName, chipsetName, m_lMid.get()));
		}

		if (layerE != nullptr) {
			layerE->changeLevel(eventLayerName);
		} else {
			layerE = LayerEPtr(new LayerE(*this, eventLayerName));
		}
		
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
	Block* b = m_lBot->getBlock(posToLookAt.x / m_blockSize, posToLookAt.y / m_blockSize);
	if (b != nullptr) {
		const unsigned int id = b->getID();
		std::vector<ska::ScriptSleepComponent*> tmp = m_chipset.getScript(ska::StringUtils::intToStr(id), reason, m_autoScriptsPlayed);
		for (auto& ssc : tmp) {
			if (ssc != nullptr) {
				ssc->context = getName();
				result.push_back(ssc);
			}
		}
		return result;
	}
	return result;
	
}

ska::Block* ska::World::getHigherBlock(const unsigned int i, const unsigned int j) {
	Block* bBot = m_lBot->getBlock(i, j);
	Block* bMid = m_lMid->getBlock(i, j);
	Block* bTop = m_lTop->getBlock(i, j);

	if (bTop != NULL && m_lTop->getBlock(i, j) != nullptr) {
		return m_lTop->getBlock(i, j);
	}

	if (bMid != NULL && m_lMid->getBlock(i, j) != nullptr) {
		return m_lMid->getBlock(i, j);
	}

	if (bBot != NULL && m_lBot->getBlock(i, j) != nullptr) {
		return m_lBot->getBlock(i, j);
	}

	return NULL;
}


int ska::World::getNbrBlocX() {
    return m_nbrBlockX;
}

unsigned int ska::World::getPixelWidth() const {
	return m_nbrBlockX*m_blockSize;
}

int ska::World::getNbrBlocY() {
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



void ska::World::getRainFromData(string stringDataFile) {
    int idsprite, acceleration, density;	
	IniReader reader(stringDataFile);

    if(reader.getString("Rain id_sprite") != "STRINGNOTFOUND") {
		idsprite = reader.getInt("Rain id_sprite");
		acceleration = reader.getInt("Rain acceleration");
		density = reader.getInt("Rain density");

    } else {
        clog << "La pluie est inexistante sur cette map" << endl;
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

    string stringDataFile = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_genericName + ""FILE_SEPARATOR"" + m_genericName + ".ini";

    this->getRainFromData(stringDataFile);
	this->getMobSettingsFromData();
}

ska::World::~World() {
}

vector<ska::IniReader>& ska::World::getMobSettings()
{
	return m_mobSettings;
}

