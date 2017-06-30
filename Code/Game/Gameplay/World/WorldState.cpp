#include "Physic/System/MovementSystem.h"
#include "Graphic/System/GraphicSystem.h"
#include "Graphic/System/ShadowSystem.h"
#include "Graphic/System/DirectionalAnimationSystem.h"
#include "Graphic/System/DeleterSystem.h"
#include "Physic/System/GravitySystem.h"

#include "Utils/StringUtils.h"
#include "Utils/RectangleUtils.h"
#include "Rectangle.h"
#include "Inputs/Readers/IniReader.h"
#include "World/Block.h"
#include "AI/IARandomMovementComponent.h"
#include "AI/IADefinedMovementComponent.h"
#include "WorldState.h"
#include "../Data/Settings.h"
#include "World/LayerE.h"
#include "../../Utils/IDs.h"
#include "Exceptions/CorruptedFileException.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "Exceptions/ScriptSyntaxError.h"
#include "../Fight/FightComponent.h"
#include "../CustomEntityManager.h"
#include "Draw/DrawableContainer.h"
#include "Core/Window.h"

WorldState::WorldState(CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::StateHolder& sh, ska::Ticked& ticked, Settings& settings) :
StateBase(entityManager, ged, w, ril, sh),
m_screenW(w.getWidth()),
m_screenH(w.getHeight()),
m_loadedOnce(false),
m_settings(settings), m_player(0),
m_saveManager(ged, "save1"),
m_cameraSystem(nullptr),
m_world(ged, TAILLEBLOC, w.getWidth(), w.getHeight()),
m_worldBGM(DEFAULT_BGM) {

	m_graphicSystem = addGraphic<ska::GraphicSystem, ska::GameEventDispatcher&, ska::CameraSystem*>(ged, nullptr);
	m_shadowSystem = addGraphic<ska::ShadowSystem, ska::CameraSystem*>(nullptr);

	addLogic<ska::DirectionalAnimationSystem>();
	addLogic<ska::InputSystem>(m_inputCManager);
	addLogic<ska::MovementSystem>(ticked);

	addLogic<ska::GravitySystem>();
	addLogic<ska::DeleterSystem>();

	m_saveManager.loadGame(m_saveManager.getPathName());
	m_worldBGM.setVolume(m_settings.getSoundVolume());
}

const std::string& WorldState::getFileName() const {
	return m_world.getFileName();
}

void WorldState::linkCamera(ska::CameraSystem* cs) {
	if (m_cameraSystem == nullptr || cs == nullptr) {
		m_cameraSystem = cs;

		m_graphicSystem->linkCamera(cs);
		m_shadowSystem->linkCamera(cs);
		m_world.linkCamera(cs);
	}
}

bool WorldState::loadedOnce() const{
	return m_loadedOnce;
}

std::vector<ska::IniReader>& WorldState::getMobSettings() {
	return m_world.getMobSettings();
}

void WorldState::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {

	//Première couche
	drawables.addHead(m_world.getLayerRenderable(0));

	//Deuxième couche
	drawables.addHead(m_world.getLayerRenderable(1));

	/* We use the maximum drawing priority of characters to draw the top layer */
	m_world.getLayerRenderable(2).setPriority(m_graphicSystem->getTopLayerPriority());
	drawables.add(m_world.getLayerRenderable(2));

	m_pokeball.setPriority(m_graphicSystem->getTopLayerPriority() + 1);
	drawables.add(m_pokeball);

	/* Hello, world */
	m_world.graphicUpdate(ellapsedTime, drawables);
}

void WorldState::onEventUpdate(unsigned int ellapsedTime) {
	m_world.update();
}

ska::World& WorldState::getWorld() {
	return m_world;
}

void WorldState::afterLoad(ska::StatePtr* lastScene) {
	ska::WorldEvent we(lastScene == nullptr ? ska::WorldEventType::WORLD_CREATE : ska::WorldEventType::WORLD_CHANGE);
	we.setBgm(m_worldBGM);
	m_eventDispatcher.ska::Observable<ska::WorldEvent>::notifyObservers(we);

	SettingsChangeEvent sce(SettingsChangeEventType::ALL, m_settings);
	m_eventDispatcher.ska::Observable<SettingsChangeEvent>::notifyObservers(sce);
}

bool WorldState::beforeUnload() {
	linkCamera(nullptr);
	return false;
}

int WorldState::spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn) {

	if (nbrSpawns == 0) {
		return 0;
	}

	std::vector<unsigned int> idBlocks;
	for (unsigned int i = 0; dataSpawn->exists("Spawn on_blockid_" + ska::StringUtils::intToStr(i)); i++) {
		idBlocks.push_back(dataSpawn->get<int>("Spawn on_blockid_" + ska::StringUtils::intToStr(i)));
	}

	const unsigned int idMob = dataSpawn->get<int>("Data id");
	const auto blockSize = m_world.getBlockSize();

	auto successfulSpawns = 0;
	auto angle = static_cast<float>((2 * M_PI* (rand() % 360)) / 360);
	for (unsigned int i = 0; i < nbrSpawns; i++) {
		const auto radius = rmin + rand() % (rmax - rmin + 1);

		ska::Point<int> dest;
		dest.x = static_cast<int>(radius*ska::NumberUtils::cosinus(angle) + pos.x);
		dest.y = static_cast<int>(radius*ska::NumberUtils::sinus(angle) + pos.y);
		dest.x = (dest.x / blockSize) * blockSize;
		dest.y = (dest.y / blockSize) * blockSize;

		ska::Rectangle boxWorld;
		boxWorld.x = 0;
		boxWorld.y = 0;
		boxWorld.w = m_world.getNbrBlocX()*blockSize;
		boxWorld.h = m_world.getNbrBlocY()*blockSize;

		std::vector<ska::Point<int>> blockColPos;
		if (ska::RectangleUtils::isPositionInBox(dest, boxWorld)) {
			auto spawnAllowed = true;
			for (unsigned int j = 0; j < idBlocks.size(); j++) {
				const auto bX = dest.x / blockSize;
				const auto bY = dest.y / blockSize;
				if (bX < m_world.getNbrBlocX() && bY < m_world.getNbrBlocY()) {
					const ska::Block* b = m_world.getHigherBlock(bX, bY);
					if (b != nullptr && b->getID() == idBlocks[i]) {
						spawnAllowed = false;
					}
				}
			}


			if (spawnAllowed) {
				auto level = rand() % (dataSpawn->get<int>("Data level_min") + dataSpawn->get<int>("Data level_max") + 1) + dataSpawn->get<int>("Data level_min");
				auto mob = m_entityManager.createCharacter(ska::Point<int>(dest.x / blockSize, dest.y / blockSize), idMob, blockSize);
				/* 0 = Predifined */
				/* 1 = Random */
				/* 2 = Fixe */
				auto type = dataSpawn->get<int>("Data path_type");
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

				auto& pc = m_entityManager.getComponent<ska::PositionComponent>(mob);
				const auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(mob);
				ska::Rectangle hitbox{ pc.x + hc.xOffset, pc.y + hc.yOffset, static_cast<int>(hc.width), static_cast<int>(hc.height) };

				const auto targetBlock = m_world.placeOnNearestPracticableBlock(hitbox, 1);
				pc.x = targetBlock.x - hc.xOffset;
				pc.y = targetBlock.y- hc.yOffset;

				successfulSpawns++;
			}
		}
		angle += static_cast<float>((2.0*M_PI) / nbrSpawns);

	}
	return successfulSpawns;
}

std::unordered_map<std::string, ska::EntityId> WorldState::reinit(const std::string& fileName, const std::string& chipsetName) {

	m_world.load(fileName, chipsetName);
	if (!m_loadedOnce) {
		ska::IniReader reader("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_saveManager.getPathName() + FILE_SEPARATOR "trainer.ini");

		ska::Point<int> startPos;
		startPos.x = reader.get<int>("Trainer start_posx");
		startPos.y = reader.get<int>("Trainer start_posy");
		auto startMapName = reader.get<std::string>("Trainer start_map_name");

		std::string buf = "." FILE_SEPARATOR "Levels" FILE_SEPARATOR;
		buf += startMapName;
		buf += FILE_SEPARATOR;
		buf += startMapName;
		buf += ".ini";

		ska::IniReader mapReader(buf);
		auto startMapChipset = mapReader.get<std::string>("Chipset file");
		if (startMapChipset == "STRINGNOTFOUND") {
			throw ska::CorruptedFileException("Erreur : impossible de trouver le nom du chipset de la map de depart");
		}

		m_player = m_entityManager.createTrainer(startPos, m_world.getBlockSize());

		m_loadedOnce = true;
	} else {
		auto& mc = m_entityManager.getComponent<ska::MovementComponent>(m_player);
		mc.ax = 0;
		mc.ay = 0;
		mc.az = 0;
		mc.vx = 0;
		mc.vy = 0;
		mc.vz = 0;
		m_entityManager.refreshEntity(m_player);
	}

	ska::Point<int> posEntityId;

	std::unordered_map<std::string, ska::EntityId> result;

	const auto blockSize = m_world.getBlockSize();
	const auto& layerE = m_world.getLayerEvent();

	//Chargement des NPC sur la map (personnages & pokémon)
	for (auto i = 0; i < layerE.getNbrLignes(); i++) {
		posEntityId.y = layerE.getBlocY(i);
		posEntityId.x = layerE.getBlocX(i);
		auto id = layerE.getID(i);
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

		const auto& params = layerE.getParam(i);
		auto totalArgs = ska::StringUtils::split(params, ',');
		if (!totalArgs.empty()) {
			ssc.args.reserve(totalArgs.size() - 1);
			for (unsigned int i1 = 1; i1 < totalArgs.size(); i1++) {
				ssc.args.push_back(ska::StringUtils::trim(totalArgs[i1]));
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

ska::EntityId WorldState::getPlayer() const{
	return m_player;
}

SavegameManager& WorldState::getSaveGame() {
	return m_saveManager;
}

WorldState::~WorldState() {
}
