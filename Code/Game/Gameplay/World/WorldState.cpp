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
#include "../../Graphic/PokemonAnimationSystem.h"
#include "World/TileWorldLoaderAggregate.h"
#include "World/LayerLoaderImage.h"
#include "World/TilesetLoaderImage.h"
#include "World/TilesetEventLoaderText.h"
#include "World/TileWorldPhysics.h"
#include "WorldLoader.h"

#include "Physic/MovementSystem.h"
#include "Core/CodeDebug/CodeDebug.h"

static constexpr auto CHIPMUNK_COLLISION_TYPE_WATER = 1;

ska::TilesetLoaderImage BuildTilesetLoader(const std::string& tilesetName) {
	return { tilesetName };
}

ska::TilesetEventLoaderText BuildTilesetEventLoader(const std::string& tilesetName) {
	return { tilesetName };
}

ska::TilesetPtr BuildTileset(unsigned int tileSize, const ska::TilesetLoader& loader, const ska::TilesetEventLoader& eventLoader) {
	auto tileset = std::make_unique<ska::Tileset>(48, loader, eventLoader);
	for (auto y = 0u; y < 4u; y++) {
		for (auto x = 0u; x < 6u; x++) {
			tileset->setPropertiesBitMask(x, y, ska::TilePropertiesType::TILE_PROPERTY_WATER);
		}
	}
	
	for (auto y = 4u; y < 8u; y++) {
		for (auto x = 0u; x < 3u; x++) {
			tileset->setPropertiesBitMask(x, y, ska::TilePropertiesType::TILE_PROPERTY_WATER);
		}
	}
	return tileset;
}

WorldState::WorldState(CustomEntityManager& em, PokemonGameEventDispatcher& ed, Settings& settings) :
	SubObserver<ska::GameEvent>(std::bind(&WorldState::onGameEvent, this, std::placeholders::_1), ed),
	m_loadedOnce(false),
	m_settings(settings), m_player(0),
	m_saveManager(em, ed, "save1"),
	m_worldBGM(DEFAULT_BGM),
	m_graphicSystem(nullptr), m_shadowSystem(nullptr), m_eventDispatcher(ed),
	m_entityManager(em),
	m_walkASM(nullptr), m_correspondanceMapper("Resources/Chipsets/corr.png"),
	m_tileset(BuildTileset(48, BuildTilesetLoader("Resources/Chipsets/chipset"), BuildTilesetEventLoader("Resources/Chipsets/chipset"))),
	m_worldFileName("Levels/" + m_saveManager.getStartMapName()),
	m_world(ed, *m_tileset, BuildWorldLoader(m_correspondanceMapper, m_worldFileName)),
	m_collisionEventSender{em, m_space, ed, m_tileset->getTileSize() } {

	m_space.setIterations(10);
	m_space.setSleepTimeThreshold(0.5F);

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

void DisplayPlayerControlBody(const ska::cp::Body& controlBody, std::vector<ska::Polygon<int>>& layerContours) {
	static bool emplaced = false;
	auto pos = ska::Point<int>{ controlBody.getPosition() };
	auto ttttt = std::vector<ska::Point<int>>{ pos, pos + ska::Point<int>{0, 10}, pos + ska::Point<int>{10, 10}, pos + ska::Point<int>{10, 0}, pos };
	if (!emplaced) {
		layerContours.emplace_back(std::move(ttttt));
		emplaced = true;
	} else {
		layerContours[layerContours.size() - 1] = std::move(ttttt);
	}
}

void WorldState::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	m_pokeball.setPriority(m_graphicSystem->getTopLayerPriority() + 1);
	drawables.add(m_pokeball);

	/* Hello, world */
	m_world.graphicUpdate(m_cameraSystem->getDisplay(), drawables);
	//TODO enlever la météo de world
	m_world.getWeather().graphicUpdate(m_cameraSystem->getDisplay(), drawables);
	m_world.getFog().graphicUpdate(m_cameraSystem->getDisplay(), drawables);

	auto& hc = m_entityManager.getComponent < ska::cp::HitboxComponent>(0);
	auto& controlBody = m_space.getBody(hc.controlBodyIndex);
	
#if 0
		DisplayPlayerControlBody(controlBody, m_layerContours);
		auto& ctrlBdyDrawable = m_layerContours[m_layerContours.size() - 1];
		ctrlBdyDrawable.setOffset(ska::Point<int> { -m_cameraSystem->getDisplay().x, -m_cameraSystem->getDisplay().y});
		drawables.add(ctrlBdyDrawable);
#endif

		

	for (auto& l : m_layerContours) {
		l.setOffset(ska::Point<int> { -m_cameraSystem->getDisplay().x, -m_cameraSystem->getDisplay().y});
		drawables.add(l);
	}

	

	for (auto& l : m_layerContoursWater) {
		l.setOffset(ska::Point<int> { -m_cameraSystem->getDisplay().x, -m_cameraSystem->getDisplay().y});
		drawables.add(l);
	}
}

void WorldState::onEventUpdate(const unsigned int timeStep) {
	m_space.step(timeStep / 1000.);
	m_tileset->update();

	if (m_cameraSystem != nullptr) {
		auto& pc = m_entityManager.getComponent<ska::PositionComponent>(0);
		auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(0);
		const auto& pos = ska::Point<int>{
			static_cast<int>(pc.x + (hc.xOffset + hc.width) / 2), 
			static_cast<int>(pc.y + (hc.yOffset + hc.height) / 2) };
		m_heroPos = ska::Point<int>{ pos.x, pos.y};
	}
}

ska::TileWorld& WorldState::getWorld() {
	return m_world;
}

bool WorldState::onGameEvent(ska::GameEvent& ge) {
	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY ||
		ge.getEventType() == ska::GameEventType::GAME_WINDOW_RESIZED) {
		m_screenSize = { static_cast<int>(ge.windowWidth), static_cast<int>(ge.windowHeight) };
		auto dbgGuiEvent = ska::DebugGUIEvent{ ska::DebugGUIEventType::ADD_DEBUG_INFO, [&]() {
			std::stringstream ss;
			const auto blockSize = m_world.getBlockSize();
			ss << (m_heroPos.x / blockSize) << "; " << (m_heroPos.y / blockSize) << " (";
			ss << m_heroPos.x << "; " << m_heroPos.y << ")";
			return ss.str();
		} };

		dbgGuiEvent.delay = 500;
		dbgGuiEvent.text = "Hero Pos : ";
		m_eventDispatcher.ska::Observable<ska::DebugGUIEvent>::notifyObservers(dbgGuiEvent);
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

	auto animSystemPtr = std::make_unique<PokemonAnimationSystem>(m_entityManager);
	auto& animSystem = *animSystemPtr.get();
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

	ska::EntityId pkmn = 1;
	auto& ac = m_entityManager.getComponent<ska::AnimationComponent>(pkmn);
	ac.setASM(*m_walkASM, pkmn);

	const auto wet = m_firstState ? ska::WorldEventType::WORLD_CREATE : ska::WorldEventType::WORLD_CHANGE;
	auto we = ska::WorldEvent{ wet };
	we.blocksWidth = m_world.getBlocksX();
	we.blocksHeight = m_world.getBlocksY();
	we.blockSize = m_world.getBlockSize();
	m_eventDispatcher.ska::Observable<ska::WorldEvent>::notifyObservers(we);
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
		auto& pShape = m_space.getShape(m_entityManager.getComponent<ska::cp::HitboxComponent>(m_player).shapeIndex);
		pShape.setBounciness(0.F);

		m_loadedOnce = true;
	} else {
		auto entitiedToNotRemoveSet = std::unordered_set<ska::EntityId>{};
		entitiedToNotRemoveSet.insert(0);
		m_entityManager.removeEntities(entitiedToNotRemoveSet);
	}

	if (getTilesetName() != chipsetName) {
		m_tileset = BuildTileset(m_tileset->getTileSize(), BuildTilesetLoader(chipsetName), BuildTilesetEventLoader(chipsetName));
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

	const auto agglomeratedTiles = GenerateAgglomeratedTileMap(1, m_world.getCollisionProfile(), [](const ska::Tile* b) {
		if (b == nullptr ||
			(b->properties.bitMask & ska::TilePropertiesType::TILE_PROPERTY_WATER == ska::TilePropertiesType::TILE_PROPERTY_WATER)) {
			return ska::TileCollision::No;
		}
		return b->collision;
	});

	const auto agglomeratedTilesWater = GenerateAgglomeratedTileMap(1, m_world.getCollisionProfile(), [](const ska::Tile* b) {
		if (b == nullptr) {
			return ska::TileCollision::No;
		}
		return (b->properties.bitMask & ska::TilePropertiesType::TILE_PROPERTY_WATER == ska::TilePropertiesType::TILE_PROPERTY_WATER) ? ska::TileCollision::Yes : ska::TileCollision::No;
	});
	const auto contourRectangleTile = GenerateContourTileMap(agglomeratedTiles);
	const auto contourRectangleTileWater = GenerateContourTileMap(agglomeratedTilesWater);

	//TODO à revoir
	//On garde le héros, donc on commence à l'index 2 bodies (control + real) 1 shape
	auto& hc = m_entityManager.getComponent<ska::cp::HitboxComponent>(0);
	if (hc.bodyIndex == hc.controlBodyIndex) {
		m_space.eraseBodies(1);
	}
	else {
		m_space.eraseBodies(2);
	}

	for (const auto& r : contourRectangleTile) {
		const auto shIndex = m_space.addShape(nullptr, ska::cp::Shape::fromBox(m_space.getStaticBody(), r));
		auto& sh = m_space.getShape(shIndex);
		sh.setBounciness(1.F);
		sh.setFriction(1.F);
	}

	for (const auto& r : contourRectangleTileWater) {
		const auto shIndex = m_space.addShape(nullptr, ska::cp::Shape::fromBox(m_space.getStaticBody(), r));
		auto& sh = m_space.getShape(shIndex);
		sh.setBounciness(1.F);
		sh.setFriction(1.F);
		sh.setCollisionType(CHIPMUNK_COLLISION_TYPE_WATER);
	}

	m_layerContours.emplace_back(contourRectangleTile);
	m_layerContoursWater.emplace_back(contourRectangleTileWater);

	auto i = 0u;
	for (auto& c : m_layerContours) {
		c.setPriority(90000 + i);
		i++;
	}

	for (auto& c : m_layerContoursWater) {
		c.setPriority(90000 + i);
		c.setColor({ 0, 0, 255, 255 });
		i++;
	}

	ska::Point<int> posEntityId;

	std::unordered_map<std::string, ska::EntityId> result;
	
	auto pkmn = CustomEntityManager::createCharacterNG(m_entityManager, m_space, { 4,5 }, 25, m_world.getBlockSize());

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

ska::cp::Space& WorldState::getSpace() {
	return m_space;
}

SavegameManager& WorldState::getSaveGame() {
	return m_saveManager;
}
