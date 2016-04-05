#include "../../ska/Utils/StringUtils.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "../../ska/Graphic/Rectangle.h"
#include "../../ska/Inputs/Readers/IniReader.h"
#include "../../ska/World/Block.h"
#include "../../ska/AI/IAMovementComponent.h"
#include "WorldScene.h"
#include "../../ska/World/LayerE.h"
#include "../../ska/World/Layer.h"
#include "../../Utils/IDs.h"
#include "../../ska/Exceptions/CorruptedFileException.h"
#include "../../ska/Script/ScriptSleepComponent.h"
#include "../../ska/Exceptions/ScriptSyntaxError.h"
#include "../Fight/FightComponent.h"
#include "../CustomEntityManager.h"

WorldScene::WorldScene(CustomEntityManager& entityManager, ska::SceneHolder& sh, ska::InputContextManager& ril, const unsigned int screenW, const unsigned int screenH) :
ska::Scene(sh, ril),
m_entityManager(entityManager),
m_saveManager("save1"),
m_world(TAILLEBLOC, screenW, screenH),
m_graphicSystem(NULL, m_entityManager),
m_shadowSystem(NULL, m_entityManager),
m_movementSystem(m_entityManager),
m_gravitySystem(m_entityManager),
m_forceSystem(m_entityManager),
m_daSystem(m_entityManager),
m_deleterSystem(m_entityManager),
m_collisionSystem(m_world, m_entityManager),
m_inputSystem(m_inputCManager, m_entityManager),
m_cameraSystem(NULL),
m_screenW(screenW),
m_screenH(screenH) {
	m_loadedOnce = false;

	m_graphics.push_back(&m_graphicSystem);
	m_graphics.push_back(&m_shadowSystem);

	m_logics.push_back(&m_inputSystem);
	m_logics.push_back(&m_movementSystem);
	m_logics.push_back(&m_gravitySystem);
	m_logics.push_back(&m_forceSystem);
	m_logics.push_back(&m_daSystem);
	m_logics.push_back(&m_deleterSystem);
	m_logics.push_back(&m_collisionSystem);

	m_saveManager.loadGame(m_saveManager.getPathName());

}

void WorldScene::linkCamera(ska::CameraSystem* cs) {
	if (m_cameraSystem == NULL || cs == NULL) {
		m_cameraSystem = cs;
		for (auto& g : m_graphics) {
			g->linkCamera(cs);
		}
		m_world.linkCamera(cs);
	}
}

bool WorldScene::loadedOnce() {
	return m_loadedOnce;
}

std::vector<ska::IniReader>& WorldScene::getMobSettings() {
	return m_world.getMobSettings();
}

void WorldScene::graphicUpdate(ska::DrawableContainer& drawables) {
	/* Hello, world */
	
	//Premi�re couche
	drawables.addHead(*m_world.getLayerBot());

	//Deuxi�me couche
	drawables.addHead(*m_world.getLayerMid());

	ska::Scene::graphicUpdate(drawables);

	drawables.addHead2D(*m_world.getLayerTop());
}

ska::World& WorldScene::getWorld() {
	return m_world;
}

void WorldScene::load() {
	
}

void WorldScene::unload() {
	linkCamera(NULL);
}

int WorldScene::spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn) {

	if (nbrSpawns == 0) {
		return 0;
	}

	std::vector<unsigned int> idBlocks;
	for (unsigned int i = 0; dataSpawn->get("Spawn on_blockid_" + ska::StringUtils::intToStr(i)); i++) {
		idBlocks.push_back(dataSpawn->getInt("Spawn on_blockid_" + ska::StringUtils::intToStr(i)));
	}

	const unsigned int idMob = dataSpawn->getInt("Data id");
	const unsigned int blockSize = m_world.getBlockSize();

	int successfulSpawns = 0;
	float angle = (float)((2 * M_PI* (rand() % 360)) / 360);
	for (unsigned int i = 0; i < nbrSpawns; i++) {
		const unsigned int radius = rmin + rand() % (rmax - rmin + 1);

		ska::Point<int> dest;
		dest.x = (int)(radius*cos(angle) + pos.x);
		dest.y = (int)(radius*sin(angle) + pos.y);
		dest.x = (dest.x / blockSize) * blockSize;
		dest.y = (dest.y / blockSize) * blockSize;

		ska::Rectangle boxWorld, boxDest;
		boxWorld.x = 0;
		boxWorld.y = 0;
		boxWorld.w = m_world.getNbrBlocX()*blockSize;
		boxWorld.h = m_world.getNbrBlocY()*blockSize;
		boxDest.x = dest.x - radius;
		boxDest.y = dest.y - radius;
		boxDest.h = boxDest.w = 30;

		if (ska::RectangleUtils::isPositionInBox(dest, boxWorld) && m_world.canMoveToPos(boxDest)) {
			bool spawnAllowed = true;
			for (unsigned int j = 0; j < idBlocks.size(); j++) {
				const ska::Block* b = m_world.getHigherBlock(dest.x / blockSize, dest.y / blockSize);
				if (b != NULL && b->getID() == idBlocks[i]) {
					spawnAllowed = false;
				}
			}


			if (spawnAllowed) {
				int level = rand() % (dataSpawn->getInt("Data level_min") + dataSpawn->getInt("Data level_max") + 1) + dataSpawn->getInt("Data level_min");
				ska::EntityId mob = m_entityManager.createCharacter(ska::Point<int>(dest.x / blockSize, dest.y / blockSize), idMob, blockSize);
				ska::IAMovementComponent iamc;
				iamc.delay = 500;
				/* 0 = Predifined */
				/* 1 = Random */
				/* 2 = Fixe */
				iamc.type = dataSpawn->getInt("Data path_type");
				if (iamc.type != 2) {
					m_entityManager.addComponent<ska::IAMovementComponent>(mob, iamc);
				}

				FightComponent fc;
				fc.level = level;
				fc.id = idMob;
				m_entityManager.addComponent<FightComponent>(mob, fc);
				successfulSpawns++;
			}
		}
		angle += (float)((2.0*M_PI) / nbrSpawns);

	}
	return successfulSpawns;
}

std::unordered_map<std::string, ska::EntityId> WorldScene::reinit(std::string fileName, std::string chipsetName) {
	
	if (!m_loadedOnce) {
		m_world.load(fileName, chipsetName);
		ska::IniReader reader("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_saveManager.getPathName() + FILE_SEPARATOR"trainer.ini");

		ska::Point<int> startPos;
		startPos.x = reader.getInt("Trainer start_posx");
		startPos.y = reader.getInt("Trainer start_posy");
		std::string startMapName = reader.getString("Trainer start_map_name");

		std::string buf = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR;
		buf += startMapName;
		buf += FILE_SEPARATOR;
		buf += startMapName;
		buf += ".ini";

		ska::IniReader mapReader(buf);
		std::string startMapChipset = mapReader.getString("Chipset file");
		if (startMapChipset == "STRINGNOTFOUND") {
			throw ska::CorruptedFileException("Erreur : impossible de trouver le nom du chipset de la map de depart");
		}
		
		m_player = m_entityManager.createTrainer(startPos, m_world.getBlockSize());

		m_loadedOnce = true;
	} else {
		m_world.changeLevel(fileName, chipsetName);
		m_entityManager.refreshEntity(m_player);
	}

	ska::Point<int> posEntityId;
	ska::Point<int> startPos;

	std::unordered_map<std::string, ska::EntityId> result;

	const unsigned int blockSize = m_world.getBlockSize();
	const ska::LayerEPtr& layerE = m_world.getLayerEvent();

	//Chargement des NPC sur la map (personnages & pok�mon)
	for (int i = 0; i < layerE->getNbrLignes(); i++)
	{
		posEntityId.y = layerE->getBlocY(i);
		posEntityId.x = layerE->getBlocX(i);
		int id = layerE->getID(i);
		if (id == 0) {
			continue;
		}

		ska::EntityId script;
		if (id == INT_MIN) {
			script = m_entityManager.createEntity();
			ska::PositionComponent pc;
			pc.x = posEntityId.x * blockSize;
			pc.y = posEntityId.y * blockSize;
			pc.z = 0;
			m_entityManager.addComponent<ska::PositionComponent>(script, pc);

		}
		else {
			if (abs(id) <= ENTITEMAX) {
				script = m_entityManager.createCharacter(posEntityId, id, blockSize);
			}
			else {
				throw ska::CorruptedFileException("Erreur (fonction LoadEntities) : Impossible de lire l'ID de l'entit� ligne " + ska::StringUtils::intToStr(i));
			}
		}

		ska::ScriptSleepComponent ssc;

		const std::string params = layerE->getParam(i);
		std::vector<std::string> totalArgs = ska::StringUtils::split(params, ',');
		if (!totalArgs.empty()) {
			ssc.args.reserve(totalArgs.size() - 1);
			for (unsigned int i = 1; i < totalArgs.size(); i++) {
				ssc.args.push_back(ska::StringUtils::trim(totalArgs[i]));
			}
		}
		else {
			throw ska::ScriptSyntaxError("Error while reading a script in the event layer file (l." + ska::StringUtils::intToStr(i) + ") : no arguments supplied to the script cmd");
		}
		ssc.name = ska::StringUtils::trim(totalArgs[0]);
		ssc.context = m_world.getName();
		ssc.triggeringType = layerE->getTrigger(i);
		ssc.period = 1000;
		m_entityManager.addComponent<ska::ScriptSleepComponent>(script, ssc);
		result[ska::StringUtils::intToStr(i + 2)] = script;
	}
	return result;
}

const unsigned int WorldScene::getScreenW() const {
	return m_screenW;
}

const unsigned int WorldScene::getScreenH() const {
	return m_screenH;
}

ska::EntityId WorldScene::getPlayer() {
	return m_player;
}

CustomEntityManager& WorldScene::getEntityManager() {
	return m_entityManager;
}

SavegameManager& WorldScene::getSaveGame() {
	return m_saveManager;
}

WorldScene::~WorldScene() {
}