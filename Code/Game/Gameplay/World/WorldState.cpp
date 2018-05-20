#include "Graphic/System/GraphicSystem.h"
#include "Graphic/System/ShadowSystem.h"
#include "Graphic/System/AnimationSystem.h"
#include "Graphic/System/DeleterSystem.h"
#include "Graphic/System/CameraFollowStrategy.h"
#include "Utils/StringUtils.h"
#include "Utils/RectangleUtils.h"
#include "Rectangle.h"
#include "Inputs/Readers/IniReader.h"
#include "AI/IARandomMovementComponent.h"
#include "AI/IADefinedMovementComponent.h"
#include "WorldState.h"
#include "../Data/Settings.h"
#include "../../Utils/IDs.h"
#include "Exceptions/CorruptedFileException.h"
#include "../Fight/FightComponent.h"
#include "../CustomEntityManager.h"
#include "Draw/DrawableContainer.h"
#include "Graphic/System/WalkAnimationStateMachine.h"

#include "World/TileWorldLoaderAggregate.h"
#include "World/LayerLoaderImage.h"
#include "World/TilesetLoaderImage.h"
#include "World/TilesetEventLoaderText.h"
#include "World/TileWorldPhysics.h"
#include "WorldLoader.h"

#include "Physic/MovementSystem.h"

ska::TilesetLoaderImage BuildTilesetLoader(const std::string& tilesetName) {
	return { tilesetName };
}

ska::TilesetEventLoaderText BuildTilesetEventLoader(const std::string& tilesetName) {
	return { tilesetName };
}

WorldState::WorldState(CustomEntityManager& em, PokemonGameEventDispatcher& ed, Settings& settings) :
	SubObserver<ska::GameEvent>(std::bind(&WorldState::onGameEvent, this, std::placeholders::_1), ed),
	m_loadedOnce(false),
	m_settings(settings), m_player(0),
	m_saveManager(ed, "save1"),
	m_worldBGM(DEFAULT_BGM),
	m_graphicSystem(nullptr), m_shadowSystem(nullptr), m_eventDispatcher(ed),
	m_entityManager(em),
	m_walkASM(nullptr), m_correspondanceMapper("Resources/Chipsets/corr.png"),
	m_tileset(std::make_unique<ska::Tileset>(48, BuildTilesetLoader("Resources/Chipsets/chipset"), BuildTilesetEventLoader("Resources/Chipsets/chipset"))),
	m_worldFileName("Levels/" + m_saveManager.getStartMapName()),
	m_world(ed, *m_tileset, BuildWorldLoader(m_correspondanceMapper, m_worldFileName)),
	m_collisionEventSender{ m_space, ed, m_tileset->getTileSize() } {
}

const std::string& WorldState::getFileName() const {
	return m_worldFileName;
}

const std::string& WorldState::getTilesetName() const {
	return m_tileset->getName();
}

bool WorldState::loadedOnce() const{
	return m_loadedOnce;
}

std::vector<ska::IniReader>& WorldState::getMobSettings() {
	return m_world.getMobSettings();
}

void WorldState::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	m_pokeball.setPriority(m_graphicSystem->getTopLayerPriority() + 1);
	drawables.add(m_pokeball);

	/* Hello, world */
	m_world.graphicUpdate(m_cameraSystem->getDisplay(), drawables);
	//TODO enlever la météo de world
	m_world.getWeather().graphicUpdate(m_cameraSystem->getDisplay(), drawables);
	m_world.getFog().graphicUpdate(m_cameraSystem->getDisplay(), drawables);

	for (auto& l : m_layerContours) {
		l.setOffset(ska::Point<int> { -m_cameraSystem->getDisplay().x, -m_cameraSystem->getDisplay().y});
		drawables.add(l);
	}
}

void WorldState::onEventUpdate(const unsigned int timeStep) {
	m_space.step(timeStep / 1000.);
	m_tileset->update();
}

ska::TileWorld& WorldState::getWorld() {
	return m_world;
}

bool WorldState::onGameEvent(ska::GameEvent& ge) {
	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY ||
		ge.getEventType() == ska::GameEventType::GAME_WINDOW_RESIZED) {
		m_screenSize = { static_cast<int>(ge.windowWidth), static_cast<int>(ge.windowHeight) };
	}

	return true;
}

void WorldState::beforeLoad(ska::State* lastState) {
}

void WorldState::afterLoad(ska::State* lastScene) {
	m_firstState = lastScene == nullptr;

	auto cameraSystem = std::make_unique<ska::CameraSystem>(m_entityManager, m_eventDispatcher, std::make_unique<ska::CameraFollowStrategy>(m_entityManager), m_screenSize.x, m_screenSize.y);
	m_cameraSystem = cameraSystem.get();
	addLogic(std::move(cameraSystem));

	auto graphicSystem = std::make_unique<ska::GraphicSystem>(m_entityManager, m_eventDispatcher, *m_cameraSystem);
	m_graphicSystem = graphicSystem.get();
	addGraphic(std::move(graphicSystem));

	auto shadowSystem = std::make_unique<ska::ShadowSystem>(m_entityManager, *m_cameraSystem);
	m_shadowSystem = shadowSystem.get();
	addGraphic(std::move(shadowSystem));

	addLogic(std::make_unique<ska::cp::MovementSystem>(m_entityManager, m_space));
	addLogic(std::make_unique<ska::InputSystem>(m_entityManager, m_eventDispatcher));
	addLogic(std::make_unique<ska::DeleterSystem>(m_entityManager));

	auto animSystemPtr = std::make_unique<ska::AnimationSystem<ska::WalkAnimationStateMachine, ska::JumpAnimationStateMachine>>(m_entityManager);
	auto& animSystem = *animSystemPtr;
	addLogic(std::move(animSystemPtr));

	m_walkASM = &animSystem.setup(true, std::make_unique<ska::WalkAnimationStateMachine>(m_entityManager));

	animSystem.setup<ska::JumpAnimationStateMachine>(false, std::make_unique<ska::JumpAnimationStateMachine>(m_entityManager));

	animSystem.link<ska::WalkAnimationStateMachine, ska::JumpAnimationStateMachine>([&](ska::EntityId& e) {
		auto& mov = m_entityManager.getComponent<ska::MovementComponent>(e);
		return ska::NumberUtils::absolute(mov.vz) > 0.1;
	});

	animSystem.link<ska::JumpAnimationStateMachine, ska::WalkAnimationStateMachine>([&](ska::EntityId& e) {
		auto& mov = m_entityManager.getComponent<ska::MovementComponent>(e);
		return ska::NumberUtils::absolute(mov.vz) <= 0.1;
	});

	SettingsChangeEvent sce(SettingsChangeEventType::ALL, m_settings);
	m_eventDispatcher.ska::Observable<SettingsChangeEvent>::notifyObservers(sce);

	reinit(m_worldFileName, getTilesetName());

	const auto wet = m_firstState ? ska::WorldEventType::WORLD_CREATE : ska::WorldEventType::WORLD_CHANGE;
	auto we = ska::WorldEvent{ wet };
	we.blocksWidth = m_world.getBlocksX();
	we.blocksHeight = m_world.getBlocksY();
	we.blockSize = m_world.getBlockSize();
	m_eventDispatcher.ska::Observable<ska::WorldEvent>::notifyObservers(we);

	if (m_firstState) {
		auto& ac = m_entityManager.getComponent<ska::AnimationComponent>(m_player);
		ac.setASM(*m_walkASM, m_player);
	}
}

void WorldState::beforeUnload() {
	m_worldBGM.setVolume(m_settings.getSoundVolume());
}

//TODO SRP
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

		ska::Rectangle boxWorld{};
		boxWorld.x = 0;
		boxWorld.y = 0;
		boxWorld.w = m_world.getPixelWidth();
		boxWorld.h = m_world.getPixelHeight();

		std::vector<ska::Point<int>> blockColPos;
		if (ska::RectangleUtils::isPositionInBox(dest, boxWorld)) {
			auto spawnAllowed = true;
			for (unsigned int j = 0; j < idBlocks.size(); j++) {
				const auto bX = dest.x / blockSize;
				const auto bY = dest.y / blockSize;
				if (bX < m_world.getBlocksX() && bY < m_world.getBlocksY()) {
					const auto* b = m_world.getCollisionProfile().getBlock(0, bX, bY);
					if (b != nullptr && (b->id.x + b->id.y * blockSize) == idBlocks[i]) {
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
					m_entityManager.addComponent<ska::IARandomMovementComponent>(mob, std::move(iamc));
				} else if (type == 0) {
					ska::IADefinedMovementComponent iamc;
					//TODO predefined paths interpreter (Up Down Left Right => positions)
					//iamc.directions.push_back()
					iamc.delay = 500;
					m_entityManager.addComponent<ska::IADefinedMovementComponent>(mob, std::move(iamc));
				}

				FightComponent fc;
				fc.level = level;
				fc.opponentScriptId = idMob;
				m_entityManager.addComponent<FightComponent>(mob, std::move(fc));

				auto& pc = m_entityManager.getComponent<ska::PositionComponent>(mob);
				const auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(mob);
				ska::Rectangle hitbox{ static_cast<int>(pc.x + hc.xOffset), static_cast<int>(pc.y + hc.yOffset), static_cast<int>(hc.width), static_cast<int>(hc.height) };

				const auto targetBlock = m_world.getCollisionProfile().placeOnNearestPracticableBlock(1, hitbox, 1);
				pc.x = targetBlock.x - hc.xOffset;
				pc.y = targetBlock.y- hc.yOffset;

				successfulSpawns++;
			}
		}
		angle += static_cast<float>((2.0*M_PI) / nbrSpawns);

	}
	return successfulSpawns;
}

//TODO SRP
std::unordered_map<std::string, ska::EntityId> WorldState::reinit(const std::string& fileName, const std::string& chipsetName) {

	if (!m_loadedOnce) {
		ska::IniReader reader("./Data/Saves/" + m_saveManager.getPathName() + "/trainer.ini");

		ska::Point<int> startPos;
		startPos.x = reader.get<int>("Trainer start_posx");
		startPos.y = reader.get<int>("Trainer start_posy");
		auto startMapName = reader.get<std::string>("Trainer start_map_name");

		std::string buf = "./Levels/";
		buf += startMapName;
		buf += "/";
		buf += startMapName;
		buf += ".ini";

		ska::IniReader mapReader(buf);
		auto startMapChipset = mapReader.get<std::string>("Chipset file");
		if (startMapChipset == "STRINGNOTFOUND") {
			throw ska::CorruptedFileException("Erreur : impossible de trouver le nom du chipset de la map de depart");
		}

		m_player = CustomEntityManager::createTrainerNG(m_entityManager, m_space, startPos, m_world.getBlockSize());

		m_loadedOnce = true;
	} 
	
	if (getTilesetName() != chipsetName) {
		m_tileset = std::make_unique<ska::Tileset>(m_tileset->getTileSize(), ska::TilesetLoaderImage{ chipsetName }, ska::TilesetEventLoaderText{ chipsetName });
	}

	if (getFileName() != fileName) {
		m_world.load(m_correspondanceMapper, fileName, m_tileset.get());
	}
	
	auto& mc = m_entityManager.getComponent<ska::MovementComponent>(m_player);
	mc.ax = 0;
	mc.ay = 0;
	mc.az = 0;
	mc.vx = 0;
	mc.vy = 0;
	mc.vz = 0;
	m_entityManager.refreshEntity(m_player);
	
	const auto agglomeratedTiles = GenerateAgglomeratedTileMap(1, m_world.getCollisionProfile());
	const auto contourRectangleTile = GenerateContourTileMap(agglomeratedTiles);

	//On garde le héros, donc on commence à l'index 1
	m_space.eraseBodies(1);
	m_space.eraseShapes(1);
	for (const auto& r : contourRectangleTile) {
		auto& sh = m_space.addShape(ska::cp::Shape::fromBox(m_space.getStaticBody(), r));
		sh.setBounciness(10.F);
	}

	m_layerContours.emplace_back(contourRectangleTile);
	auto i = 0u;
	for (auto& c : m_layerContours) {
		c.setPriority(90000 + i);
		i++;
	}

	ska::Point<int> posEntityId;

	std::unordered_map<std::string, ska::EntityId> result;


	//Chargement des NPC sur la map (personnages & pokémon)
	/*
	TODO
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
			m_entityManager.addComponent<ska::PositionComponent>(script, std::move(pc));

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
		m_entityManager.addComponent<ska::ScriptSleepComponent>(script, std::move(ssc));
		result[ska::StringUtils::intToStr(i + 2)] = script;
	}
	*/
	return result;
}

ska::EntityId WorldState::getPlayer() const{
	return m_player;
}

SavegameManager& WorldState::getSaveGame() {
	return m_saveManager;
}
