#include "../../ska/Utils/StringUtils.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "../../ska/Graphic/Rectangle.h"
#include "../../ska/Inputs/Readers/IniReader.h"
#include "../../ska/World/Block.h"
#include "../../ska/AI/IARandomMovementComponent.h"
#include "../../ska/AI/IADefinedMovementComponent.h"
#include "WorldScene.h"
#include "../Data/Settings.h"
#include "../../ska/World/LayerE.h"
#include "../../ska/World/Layer.h"
#include "../../Utils/IDs.h"
#include "../../ska/Exceptions/CorruptedFileException.h"
#include "../../ska/Script/ScriptSleepComponent.h"
#include "../../ska/Exceptions/ScriptSyntaxError.h"
#include "../Fight/FightComponent.h"
#include "../CustomEntityManager.h"
#include "../../ska/Graphic/GUI/Window.h"
#include "../../ska/Graphic/Draw/DrawableContainer.h"

WorldScene::WorldScene(CustomEntityManager& entityManager, ska::SceneHolder& sh, ska::InputContextManager& ril, ska::Window& w, Settings& settings, PokemonGameEventDispatcher& ged) :
Scene(sh, ril),
m_entityManager(entityManager),
m_saveManager(ged, "save1"),
m_world(ged, TAILLEBLOC, w.getWidth(), w.getHeight()),
m_graphicSystem(nullptr, m_entityManager),
m_shadowSystem(nullptr, m_entityManager),
m_movementSystem(m_entityManager),
m_gravitySystem(m_entityManager),
m_forceSystem(m_entityManager),
m_daSystem(m_entityManager),
m_deleterSystem(m_entityManager),
m_inputSystem(m_inputCManager, m_entityManager),
m_cameraSystem(nullptr),
m_screenW(w.getWidth()),
m_screenH(w.getHeight()),
m_gui(w, ril, ged),
m_settings(settings), m_player(0),
	m_worldBGM(DEFAULT_BGM),
m_ged(ged) {
	m_loadedOnce = false;

	m_graphics.push_back(&m_graphicSystem);
	m_graphics.push_back(&m_shadowSystem);

	m_logics.push_back(&m_inputSystem);
	m_logics.push_back(&m_movementSystem);
	m_logics.push_back(&m_gravitySystem);
	m_logics.push_back(&m_forceSystem);
	m_logics.push_back(&m_daSystem);
	m_logics.push_back(&m_deleterSystem);

	m_saveManager.loadGame(m_saveManager.getPathName());
	m_worldBGM.setVolume(m_settings.getSoundVolume());
	m_gui.bind(m_settings);
}

const std::string& WorldScene::getFileName() const {
	return m_world.getFileName();
}

void WorldScene::linkCamera(ska::CameraSystem* cs) {
	if (m_cameraSystem == nullptr || cs == nullptr) {
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

	//Première couche
	drawables.addHead(m_world.getLayerRenderable(0));

	//Deuxième couche
	drawables.addHead(m_world.getLayerRenderable(1));

	Scene::graphicUpdate(drawables);
	
	/* We use the maximum drawing priority of characters to draw the top layer */
	m_world.getLayerRenderable(2).setPriority(m_graphicSystem.getTopLayerPriority());
	drawables.add(m_world.getLayerRenderable(2));

	//WGameCore& wScreen = WGameCore::getInstance();
	m_pokeball.setPriority(m_graphicSystem.getTopLayerPriority() + 1);
	drawables.add(m_pokeball);

	/* Hello, world */
	m_world.graphicUpdate(drawables);

	//Affiche la GUI
	drawables.add(m_gui);
}

void WorldScene::eventUpdate(bool movingDisallowed) {
	m_world.update();
	
	//GUI
	m_gui.refresh();

	return Scene::eventUpdate(movingDisallowed);
}

ska::World& WorldScene::getWorld() {
	return m_world;
}

void WorldScene::load(ska::ScenePtr* lastScene) {
	ska::WorldEvent we(lastScene == nullptr ? ska::WorldEventType::WORLD_CREATE : ska::WorldEventType::WORLD_CHANGE);
	we.setBgm(m_worldBGM);
	m_ged.ska::Observable<ska::WorldEvent>::notifyObservers(we);

	SettingsChangeEvent sce(SettingsChangeEventType::ALL, m_settings);
	m_ged.ska::Observable<SettingsChangeEvent>::notifyObservers(sce);
}

bool WorldScene::unload() {
	linkCamera(nullptr);
	return false;
}

int WorldScene::spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn) {

	if (nbrSpawns == 0) {
		return 0;
	}

	std::vector<unsigned int> idBlocks;
	for (unsigned int i = 0; dataSpawn->exists("Spawn on_blockid_" + ska::StringUtils::intToStr(i)); i++) {
		idBlocks.push_back(dataSpawn->get<int>("Spawn on_blockid_" + ska::StringUtils::intToStr(i)));
	}

	const unsigned int idMob = dataSpawn->get<int>("Data id");
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

		std::vector<ska::Point<int>> blockColPos;
		if (ska::RectangleUtils::isPositionInBox(dest, boxWorld)) {
			bool spawnAllowed = true;
			for (unsigned int j = 0; j < idBlocks.size(); j++) {
				const unsigned int bX = dest.x / blockSize;
				const unsigned int bY = dest.y / blockSize;
				if (bX < m_world.getNbrBlocX() && bY < m_world.getNbrBlocY()) {
					const ska::Block* b = m_world.getHigherBlock(bX, bY);
					if (b != NULL && b->getID() == idBlocks[i]) {
						spawnAllowed = false;
					}
				}
			}


			if (spawnAllowed) {
				int level = rand() % (dataSpawn->get<int>("Data level_min") + dataSpawn->get<int>("Data level_max") + 1) + dataSpawn->get<int>("Data level_min");
				ska::EntityId mob = m_entityManager.createCharacter(ska::Point<int>(dest.x / blockSize, dest.y / blockSize), idMob, blockSize);
				/* 0 = Predifined */
				/* 1 = Random */
				/* 2 = Fixe */
				int type = dataSpawn->get<int>("Data path_type");
				if (type == 1) {
					ska::IARandomMovementComponent iamc;
					iamc.delay = 500;
					m_entityManager.addComponent<ska::IARandomMovementComponent>(mob, iamc);
				} else if (type == 0) {
					ska::IADefinedMovementComponent iamc;
					//TODO predefined paths interpreter (Up Down Left Right => positions)
					//iamc.directions.push_back()
					iamc.delay = 500;
					m_entityManager.addComponent<ska::IADefinedMovementComponent>(mob, iamc);
				}

				FightComponent fc;
				fc.level = level;
				fc.opponentScriptId = idMob;
				m_entityManager.addComponent<FightComponent>(mob, fc);

				ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(mob);
				const ska::HitboxComponent& hc = m_entityManager.getComponent<ska::HitboxComponent>(mob);
				ska::Rectangle hitbox{ pc.x + hc.xOffset, pc.y + hc.yOffset, hc.width, hc.height };

				const ska::Rectangle targetBlock = m_world.placeOnNearestPracticableBlock(hitbox, 1);
				pc.x = targetBlock.x - hc.xOffset;
				pc.y = targetBlock.y- hc.yOffset;

				successfulSpawns++;
			}
		}
		angle += (float)((2.0*M_PI) / nbrSpawns);

	}
	return successfulSpawns;
}

std::unordered_map<std::string, ska::EntityId> WorldScene::reinit(std::string fileName, std::string chipsetName) {
	
	m_world.load(fileName, chipsetName);
	if (!m_loadedOnce) {
		ska::IniReader reader("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_saveManager.getPathName() + FILE_SEPARATOR"trainer.ini");

		ska::Point<int> startPos;
		startPos.x = reader.get<int>("Trainer start_posx");
		startPos.y = reader.get<int>("Trainer start_posy");
		std::string startMapName = reader.get<std::string>("Trainer start_map_name");

		std::string buf = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR;
		buf += startMapName;
		buf += FILE_SEPARATOR;
		buf += startMapName;
		buf += ".ini";

		ska::IniReader mapReader(buf);
		std::string startMapChipset = mapReader.get<std::string>("Chipset file");
		if (startMapChipset == "STRINGNOTFOUND") {
			throw ska::CorruptedFileException("Erreur : impossible de trouver le nom du chipset de la map de depart");
		}
		
		m_player = m_entityManager.createTrainer(startPos, m_world.getBlockSize());

		m_loadedOnce = true;
	} else {
		m_entityManager.refreshEntity(m_player);
	}

	ska::Point<int> posEntityId;
	ska::Point<int> startPos;

	std::unordered_map<std::string, ska::EntityId> result;

	const unsigned int blockSize = m_world.getBlockSize();
	const ska::LayerE& layerE = m_world.getLayerEvent();

	//Chargement des NPC sur la map (personnages & pokémon)
	for (int i = 0; i < layerE.getNbrLignes(); i++) {
		posEntityId.y = layerE.getBlocY(i);
		posEntityId.x = layerE.getBlocX(i);
		int id = layerE.getID(i);
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

		} else {
			if (abs(id) <= ENTITEMAX) {
				script = m_entityManager.createCharacter(posEntityId, id, blockSize);
			} else {
				throw ska::CorruptedFileException("Erreur (fonction LoadEntities) : Impossible de lire l'ID de l'entité ligne " + ska::StringUtils::intToStr(i));
			}
		}

		ska::ScriptSleepComponent ssc;

		const std::string& params = layerE.getParam(i);
		std::vector<std::string> totalArgs = ska::StringUtils::split(params, ',');
		if (!totalArgs.empty()) {
			ssc.args.reserve(totalArgs.size() - 1);
			for (unsigned int i = 1; i < totalArgs.size(); i++) {
				ssc.args.push_back(ska::StringUtils::trim(totalArgs[i]));
			}
		} else {
			throw ska::ScriptSyntaxError("Error while reading a script in the event layer file (l." + ska::StringUtils::intToStr(i) + ") : no arguments supplied to the script cmd");
		}
		ssc.name = ska::StringUtils::trim(totalArgs[0]);
		ssc.context = m_world.getName();
		ssc.triggeringType = static_cast<ska::ScriptTriggerType>(layerE.getTrigger(i));
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
	//removeObserver(m_soundEvents);
}
