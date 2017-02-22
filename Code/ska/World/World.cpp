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

unsigned int ska::World::getNumberLayers() const {
	return 3;
}

bool ska::World::isSameBlockId(const ska::Point<int>& p1, const ska::Point<int>& p2, int layerIndex) {
	Layer* l;
	switch (layerIndex) {
	case 0:
		l = &m_lBot;
		break;
	case 1:
		l = &m_lMid;
		break;
	case 2:
		l = &m_lTop;
	default :
		l = nullptr;
		break;
	}

	const auto p1Block = p1 / m_blockSize;
	const auto p2Block = p2 / m_blockSize;
	if (p1Block.x >= m_nbrBlockX || p2Block.x >= m_nbrBlockX || p1Block.y >= m_nbrBlockY || p2Block.y >= m_nbrBlockY) {
		return true;
	}

	Block* b1 = l == nullptr ? getHigherBlock(p1Block.x, p1Block.y) : l->getBlock(p1Block.x, p1Block.y);
	Block* b2 = l == nullptr ? getHigherBlock(p2Block.x, p2Block.y) : l->getBlock(p2Block.x, p2Block.y);
	return (b1 == b2 || b1 != nullptr && b2 != nullptr && b1->getID() == b2->getID());
}

bool ska::World::canMoveOnBlock(const ska::Point<int>& pos, const std::unordered_set<int>& authorizedBlocks, int layerIndex) const {
	const Layer* l;
	switch (layerIndex) {
	case 0:
		l = &m_lBot;
		break;
	case 1:
		l = &m_lMid;
		break;
	case 2:
		l = &m_lTop;
		break;
	default:
		l = nullptr;
		break;
	}
	const ska::Point<int> blockPos = pos / m_blockSize;
	if (blockPos.x >= m_nbrBlockX || blockPos.y >= m_nbrBlockY ) {
		return true;
	}
	Block* b = layerIndex == -1 ? getHigherBlock(blockPos.x, blockPos.y) : l->getBlock(blockPos.x, blockPos.y);
	const bool result = b != nullptr ? (authorizedBlocks.find(b->getID()) != authorizedBlocks.end()) : false;
	return result;
}

ska::ChipsetHolder& ska::World::getChipset() {
	return m_chipset;
}

const std::string& ska::World::getFileName() const {
	return m_fileName;
}

bool ska::World::isBlockDodgeable(const int i, const int j) const {
	Block* b = m_lMid.getBlock(i, j);
	return (b != nullptr && b->getProperties() == BLOCK_PROP_JUMPWALL);
}

bool ska::World::getCollision(const int i, const int j) const {
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

bool ska::World::canMoveToPos(ska::Rectangle hitbox, std::vector<ska::Point<int>>& output) const {
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

	bool col = false;
	for (int y = chg.y / m_blockSize; y <= yLimit; y++) {
		for (int x = chg.x / m_blockSize; x <= xLimit; x++) {
			if (getCollision(x, y) /*&& !isBlockDodgeable(i, j)*/) {
				output.push_back(ska::Point<int>(x * m_blockSize, y * m_blockSize));
				col = true;
			}
		}
	}
	return !col;
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

std::vector<ska::ScriptSleepComponent*> ska::World::chipsetScript(const ska::Point<int>& oldPos, const ska::Point<int>& newPos, const ska::Point<int>& posToLookAt, const ScriptTriggerType& reason, const unsigned int layerIndex) {
	std::vector<ska::ScriptSleepComponent*> result;
	Layer* l;
	switch (layerIndex) {
	case 0:
		l = &m_lBot;
		break;
	case 1:
		l = &m_lMid;
		break;
	default:
		l = &m_lTop;
		break;
	}

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
	
	const ska::Point<int> newBlock = newPos / m_blockSize;
	const ska::Point<int> oldBlock = oldPos / m_blockSize;
	Block* b = l->getBlock(posToLookAt.x / m_blockSize, posToLookAt.y / m_blockSize);
	if (b != nullptr) {
		const unsigned int id = b->getID();
		std::vector<ska::ScriptSleepComponent*> tmp = m_chipset.getScript(ska::StringUtils::intToStr(id), reason, m_autoScriptsPlayed);
		for (auto& ssc : tmp) {
			if (ssc != nullptr) {
				ssc->args.clear();
				ssc->args.push_back(ska::StringUtils::intToStr(oldBlock.x));
				ssc->args.push_back(ska::StringUtils::intToStr(oldBlock.y));
				ssc->args.push_back(ska::StringUtils::intToStr(newBlock.x));
				ssc->args.push_back(ska::StringUtils::intToStr(newBlock.y));
				ssc->args.push_back(ska::StringUtils::intToStr(ska::RectangleUtils::getDirectionFromPos(oldBlock * m_blockSize, newBlock * m_blockSize)));
				ssc->context = getName();
				result.push_back(ssc);
			}
		}
	}
	return result;
	
}

ska::Point<int> ska::World::alignOnBlock(const ska::Rectangle& hitbox) const {
	ska::Point<int> hitBoxBlock = (ska::Point<int>(hitbox) / m_blockSize) * m_blockSize;
	return ska::Point<int>(hitbox) - hitBoxBlock;
}

ska::Rectangle ska::World::placeOnNearestPracticableBlock(const ska::Rectangle& hitBox, const unsigned int radius) const {
	std::vector<ska::Rectangle> blocksPos;
	ska::Point<int> hitBoxBlock = (ska::Point<int>(hitBox) + ska::Point<int>(hitBox.w, hitBox.h) / 2) / m_blockSize;
	ska::Rectangle result = hitBox;
	

	if (radius == 0) {
		return result;
	}

	const unsigned int maxWidth = getNbrBlocX();
	const unsigned int maxHeight = getNbrBlocY();

	ska::Rectangle blockArea;
	blockArea.x = hitBoxBlock.x - radius;
	blockArea.y = hitBoxBlock.y - radius;
	blockArea.w = (radius << 1) + 1;
	blockArea.h = (radius << 1) + 1;
	
	if (blockArea.x < 0) {
		blockArea.x = 0;
	}

	if (blockArea.y < 0) {
		blockArea.y = 0;
	}

	if (blockArea.x + 1 > (int) maxWidth) {
		blockArea.x = maxWidth - 1;
	}

	if (blockArea.y + 1 > (int) maxHeight) {
		blockArea.y = maxHeight - 1;
	}

	if (blockArea.x + blockArea.w > (int) maxWidth) {
		blockArea.w = maxWidth - blockArea.x;
	}

	if (blockArea.y + blockArea.h > (int) maxHeight) {
		blockArea.h = maxHeight - blockArea.y;
	}

	for (unsigned int x = 0; x != blockArea.w; x++) {
		for (unsigned int y = 0; y != blockArea.h; y++) {
			ska::Rectangle rect{ x + blockArea.x, y + blockArea.y, hitBox.w, hitBox.h};
			blocksPos.push_back(rect);
		}
	}

	auto x = *blocksPos.begin();
	std::sort(blocksPos.begin(), blocksPos.end(), [hitBoxBlock](const decltype(x)& it1, decltype(x)& it2) -> bool {
		return ska::RectangleUtils::distanceSquared(it1, hitBoxBlock) < ska::RectangleUtils::distanceSquared(it2, hitBoxBlock);
	});

	for (const auto& r : blocksPos) {
		if (!getCollision(r.x, r.y)) {
			result = r;
			result.x *= m_blockSize;
			result.y *= m_blockSize;
			break;
		}
	}

	return result;
}

ska::Block* ska::World::getHigherBlock(const unsigned int i, const unsigned int j) const {
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

    if(reader.exists("Rain id_sprite")) {
		idsprite = reader.get<int>("Rain id_sprite");
		acceleration = reader.get<int>("Rain acceleration");
		density = reader.get<int>("Rain density");

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

	//le dernier �l�ment est invalide, on le supprime donc
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

