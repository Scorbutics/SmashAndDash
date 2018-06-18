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
#include "../../Script/ScriptConstants.h"
#include "World/TileWorldLoaderAggregate.h"
#include "World/LayerLoaderImage.h"
#include "World/TilesetLoaderImage.h"
#include "World/TilesetEventLoaderText.h"
#include "World/TileWorldPhysics.h"
#include "WorldLoader.h"
#include "Physic/SpaceSystem.h"
#include "Physic/MovementSystem.h"
#include "Core/CodeDebug/CodeDebug.h"
#include "Physic/ZGravityForceSystem.h"

//#define PKMN_DRAW_DBG_SHAPES

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
	m_settings(settings), 
	m_saveManager(em, ed, "save1"),
	m_worldBGM(DEFAULT_BGM), 
	m_eventDispatcher(ed),
	m_entityManager(em),
	m_entityLocator(ed),
	m_correspondanceMapper("Resources/Chipsets/corr.png"),
	m_tileset(BuildTileset(48, BuildTilesetLoader("Resources/Chipsets/chipset"), BuildTilesetEventLoader("Resources/Chipsets/chipset"))),
	m_worldFileName("Levels/" + m_saveManager.getStartMapName()),
	m_world(ed, *m_tileset, BuildWorldLoader(m_correspondanceMapper, m_worldFileName)),
	m_debugDrawer(ed, em, m_entityLocator, m_tileset->getTileSize()) {

	ska::IniReader reader("./Data/Saves/" + m_saveManager.getPathName() + "/trainer.ini");

	m_posHero.x = reader.get<int>("Trainer start_posx");
	m_posHero.y = reader.get<int>("Trainer start_posy");
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

#ifdef PKMN_DRAW_DBG_SHAPES
#ifdef PKMN_DRAW_CONTROL_BODY
void DisplayPlayerControlBody(const ska::cp::Body& controlBody, std::vector<ska::Polygon<int>>& layerContours) {
	static bool emplaced = false;
	auto pos = ska::Point<int>{ controlBody.getPosition() };
	auto ttttt = std::vector<ska::Point<int>>{ pos, pos + ska::Point<int>{0, 10}, pos + ska::Point<int>{10, 10}, pos + ska::Point<int>{10, 0}, pos };
	if (!emplaced) {
		layerContours.emplace_back(std::move(ttttt));
		emplaced = true;
	}
	else {
		layerContours[layerContours.size() - 1] = std::move(ttttt);
	}
}
#endif
#endif

void WorldState::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	m_debugDrawer.graphicUpdate(ellapsedTime);

	m_pokeball.setPriority(m_graphicSystem->getTopLayerPriority() + 1);
	drawables.add(m_pokeball);

	/* Hello, world */
	m_world.graphicUpdate(m_cameraSystem->getDisplay(), drawables);
	//TODO enlever la météo de world
	m_world.getWeather().graphicUpdate(m_cameraSystem->getDisplay(), drawables);
	m_world.getFog().graphicUpdate(m_cameraSystem->getDisplay(), drawables);

	const auto* trainer = m_entityLocator.getEntityId(SCRIPT_ENTITY_TRAINER);
	{
		if (trainer != nullptr) {		
			m_posHeroPolygon.setOffset(ska::Point<int> { static_cast<int>(m_posHero.x - m_cameraSystem->getDisplay().x), static_cast<int>(m_posHero.y - m_cameraSystem->getDisplay().y)});
			m_posHeroPolygon.setPriority(std::numeric_limits<int>::max());
			drawables.add(m_posHeroPolygon);
		}
	}
	
#ifdef PKMN_DRAW_DBG_SHAPES
#ifdef PKMN_DRAW_CONTROL_BODY
	const auto& hc = m_entityManager.getComponent < ska::cp::HitboxComponent>(*trainer);
	const auto& controlBody = m_space.getBody(hc.controlBodyIndex);

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
#endif
}

void WorldState::onEventUpdate(const unsigned int timeStep) {
	m_tileset->update();
	m_debugDrawer.eventUpdate(timeStep);
}

const ska::EntityLocator& WorldState::getEntityLocator() const {
	return m_entityLocator;
}

ska::TileWorld& WorldState::getWorld() {
	return m_world;
}

bool WorldState::onGameEvent(ska::GameEvent& ge) {
	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY ||
		ge.getEventType() == ska::GameEventType::GAME_WINDOW_RESIZED) {

		m_screenSize = { static_cast<int>(ge.windowWidth), static_cast<int>(ge.windowHeight) };
		m_debugDrawer.setupUI();
	}

	return true;
}

void WorldState::beforeLoad(ska::State* lastState) {
	m_firstState = lastState == nullptr;

	auto cameraSystem = std::make_unique<ska::CameraSystem>(m_entityManager, m_eventDispatcher, std::make_unique<ska::CameraFollowStrategy>(m_entityManager), m_screenSize.x, m_screenSize.y);
	m_cameraSystem = cameraSystem.get();
	addLogic(std::move(cameraSystem));

	auto graphicSystem = std::make_unique<ska::GraphicSystem>(m_entityManager, m_eventDispatcher, *m_cameraSystem);
	m_graphicSystem = graphicSystem.get();
	addGraphic(std::move(graphicSystem));

	auto shadowSystem = std::make_unique<ska::ShadowSystem>(m_entityManager, *m_cameraSystem);
	m_shadowSystem = shadowSystem.get();
	addGraphic(std::move(shadowSystem));
	
	auto spaceSystem = std::make_unique<ska::cp::SpaceSystem>(m_entityManager);
	m_spaceSystem = spaceSystem.get();
	addLogic(std::move(spaceSystem));

	addLogic(std::make_unique<ska::cp::MovementSystem>(m_entityManager, m_spaceSystem->getSpace()));
	addLogic(std::make_unique<ska::InputSystem>(m_entityManager, m_eventDispatcher));
	addLogic(std::make_unique<ska::DeleterSystem>(m_entityManager));
	addLogic(std::make_unique<ska::ZGravityForceSystem>(m_entityManager, 0.7F));
	auto animSystemPtr = std::make_unique<PokemonAnimationSystem>(m_entityManager);
	auto& animSystem = *animSystemPtr.get();
	addLogic(std::move(animSystemPtr));

	m_walkASM = &animSystem.setup(true, std::make_unique<ska::WalkAnimationStateMachine>(m_entityManager));

	animSystem.setup<ska::JumpAnimationStateMachine>(false, std::make_unique<ska::JumpAnimationStateMachine>(m_entityManager));

	animSystem.link<ska::WalkAnimationStateMachine, ska::JumpAnimationStateMachine>([&](const ska::EntityId& e) {
		auto& mov = m_entityManager.getComponent<ska::MovementComponent>(e);
		return mov.vz > 0.1;
	});

	animSystem.link<ska::JumpAnimationStateMachine, ska::WalkAnimationStateMachine>([&](const ska::EntityId& e) {
		auto& mov = m_entityManager.getComponent<ska::MovementComponent>(e);
		return mov.vz <= 0.1;
	});

	SettingsChangeEvent sce(SettingsChangeEventType::ALL, m_settings);
	m_eventDispatcher.ska::Observable<SettingsChangeEvent>::notifyObservers(sce);

	m_collisionEventSender = std::make_unique<ska::cp::SpaceCollisionEventSender>(m_entityManager, m_spaceSystem->getSpace(), m_eventDispatcher, m_tileset->getTileSize());
}

void WorldState::afterLoad(ska::State* lastState) {
	
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
void WorldState::reinit(const std::string& fileName, const std::string& chipsetName) {
	//DEBUT CLEAN UP
#ifdef PKMN_DRAW_DBG_SHAPES
	m_layerContours.clear();
	m_layerContoursWater.clear();
#endif

	m_spaceSystem->reset();
	
	auto trainer = m_entityLocator.getEntityId("trainer");
	if (trainer != nullptr) {
		auto donotDelete = std::unordered_set<ska::EntityId>{};
		donotDelete.emplace(*trainer);

		m_entityManager.removeEntities(donotDelete);
		m_entityManager.refresh();
	}
	//FIN CLEAN UP

	//DEBUT WORLD
	if (getTilesetName() != chipsetName) {
		m_tileset = BuildTileset(m_tileset->getTileSize(), BuildTilesetLoader(chipsetName), BuildTilesetEventLoader(chipsetName));
	}

	if (getFileName() != fileName) {
		m_world.load(m_correspondanceMapper, fileName, m_tileset.get());
	}
	//FIN WORLD

	//DEBUT PLAYER
	auto player = ska::EntityId{};
	if (trainer == nullptr) {
		player = CustomEntityManager::createTrainerNG(m_entityManager, m_spaceSystem->getSpace(), m_posHero, m_world.getBlockSize());
		auto& pShape = m_spaceSystem->getSpace().getShape(m_entityManager.getComponent<ska::cp::HitboxComponent>(player).shapeIndex);
		pShape.setBounciness(0.F);
	} else {
		player = *trainer;
		CustomEntityManager::fillCharacter(m_entityManager, m_spaceSystem->getSpace(), player);
	}
	//FIN PLAYER

	auto hitboxHero = std::vector<ska::Rectangle> {};
	const auto& hcHero = m_entityManager.getComponent<ska::HitboxComponent>(player);
	hitboxHero.push_back({ 0, 0, static_cast<int>(1), static_cast<int>(1) });
	m_posHeroPolygon = ska::Polygon<int>{ std::move(hitboxHero) };

	//DEBUT Tile map
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

	for (const auto& r : contourRectangleTile) {
		const auto shIndex = m_spaceSystem->getSpace().addShape(nullptr, ska::cp::Shape::fromBox(m_spaceSystem->getSpace().getStaticBody(), r));
		auto& sh = m_spaceSystem->getSpace().getShape(shIndex);
		sh.setBounciness(1.F);
		sh.setFriction(1.F);
	}

	for (const auto& r : contourRectangleTileWater) {
		const auto shIndex = m_spaceSystem->getSpace().addShape(nullptr, ska::cp::Shape::fromBox(m_spaceSystem->getSpace().getStaticBody(), r));
		auto& sh = m_spaceSystem->getSpace().getShape(shIndex);
		sh.setBounciness(1.F);
		sh.setFriction(1.F);
		sh.setCollisionType(CHIPMUNK_COLLISION_TYPE_WATER);
	}

#ifdef PKMN_DRAW_DBG_SHAPES
	m_layerContours.emplace_back(contourRectangleTile);
	m_layerContoursWater.emplace_back(contourRectangleTileWater);
#endif

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

	//FIN Tile map
	ska::Point<int> posEntityId;
	auto pkmn = CustomEntityManager::createCharacterNG(m_entityManager, m_spaceSystem->getSpace(), { 4,5 }, 25, m_world.getBlockSize(), "pikachu");
	auto& ac = m_entityManager.getComponent<ska::AnimationComponent>(pkmn);
	ac.setASM(*m_walkASM, pkmn);

	const auto wet = m_firstState ? ska::WorldEventType::WORLD_CREATE : ska::WorldEventType::WORLD_CHANGE;
	auto we = ska::WorldEvent{ wet };
	we.blocksWidth = m_world.getBlocksX();
	we.blocksHeight = m_world.getBlocksY();
	we.blockSize = m_world.getBlockSize();
	m_eventDispatcher.ska::Observable<ska::WorldEvent>::notifyObservers(we);


	//Chargement des NPC sur la map (personnages & pokémon)
	/*
	TODO SRP
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
}

ska::cp::Space& WorldState::getSpace() {
	assert(m_spaceSystem != nullptr && "Bad instantiation of WorldState : state not loaded but trying to access its members");
	return m_spaceSystem->getSpace();
}

SavegameManager& WorldState::getSaveGame() {
	return m_saveManager;
}
