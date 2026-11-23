#include "Game/__internalConfig/LoggerConfig.h"
#include "Graphic/System/GraphicSystem.h"
#include "Graphic/Texture.h"
#include "Graphic/System/ShadowSystem.h"
#include "Graphic/System/AnimationSystem.h"
#include "Graphic/System/DeleterSystem.h"
#include "Graphic/System/CameraFollowStrategy.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Core/Utils/RectangleUtils.h"
#include "Core/Rectangle.h"
#include "Base/IO/Readers/IniReader.h"
#include "AI/IARandomMovementComponent.h"
#include "AI/IADefinedMovementComponent.h"
#include "WorldState.h"
#include "../Control/GridMovementComponent.h"
#include "../Data/Settings.h"
#include "../../Utils/IDs.h"
#include "Core/Exceptions/CorruptedFileException.h"
#include "../Fight/FightComponent.h"
#include "../CustomEntityManager.h"
#include "Core/Draw/DrawableContainer.h"
#include "../../Graphic/PokemonAnimationSystem.h"
#include "../../Script/ScriptConstants.h"
#include "World/TileWorldLoaderAggregate.h"
#include "World/LayerLoaderImage.h"
#include "World/TilesetLoaderImage.h"
#include "World/TilesetEventLoaderText.h"
#include "Scripting/ScriptVM.h"
#include "Scripting/EngineModule.h"
#include "../Fight/BattleRules.h"
#include "../Data/Team.h"
#include "../Control/InputHooks.h"
#include "Graphic/GraphicComponent.h"
#include "../../Script/GameScriptModule.h"
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
	m_bag(ed),
	m_worldBGM(DEFAULT_BGM), 
	m_eventDispatcher(ed),
	m_entityManager(em),
	m_entityLocator(ed),
	m_entityHandles(ed),
	m_correspondanceMapper("Resources/Chipsets/corr.png"),
	m_tileset(BuildTileset(48, BuildTilesetLoader("Resources/Chipsets/chipset"), BuildTilesetEventLoader("Resources/Chipsets/chipset"))),
	m_worldFileName("Levels/" + m_saveManager.getStartMapName()),
	m_world(ed, *m_tileset, BuildWorldLoader(m_correspondanceMapper, m_worldFileName)),
	m_debugDrawer(ed, em, m_entityLocator, m_tileset->getTileSize()) {

	m_scripts = std::make_unique<ska::scripting::ScriptVM>();
	m_engineScriptModule = std::make_unique<ska::scripting::EngineModule>(*m_scripts, m_entityManager, m_entityLocator, m_entityHandles, &m_saveManager);
	// Scripts can hide a character (Engine.disableComponent(character, "GraphicComponent"))
	m_engineScriptModule->registerComponent<ska::GraphicComponent>("GraphicComponent");
	m_gameScriptModules = std::make_unique<GameScriptModules>(*m_scripts, m_entityManager, m_eventDispatcher, *this);
	m_battleRules = std::make_unique<BattleRules>(*m_scripts);
	m_scripts->declareEvent("battle.won");
	m_scripts->declareEvent("battle.lost");
	m_scripts->declareEvent("battle.fled");
	m_scripts->declareEvent(InputHooks::Action);
	m_scripts->declareEvent(InputHooks::Jump);

	ska::IniReader reader("./Data/Saves/" + m_saveManager.getPathName() + "/trainer.ini");

	m_posHero.x = reader.get<int>("Trainer start_posx");
	m_posHero.y = reader.get<int>("Trainer start_posy");
	auto startMapName = reader.get<std::string>("Trainer start_map_name");

	const auto buf = "./Levels/" + startMapName + "/" + startMapName + ".ini";
	ska::IniReader mapReader(buf);
	mapReader.get<std::string>("Chipset file");
}

const std::string& WorldState::getFileName() const {
	return m_worldFileName;
}

std::string WorldState::getMapName() const {
	auto name = m_worldFileName;
	while (!name.empty() && (name.back() == '/' || name.back() == '\\')) {
		name.pop_back();
	}
	const auto separator = name.find_last_of("/\\");
	return separator == std::string::npos ? name : name.substr(separator + 1);
}

const std::string& WorldState::getTilesetName() const {
	return m_tileset->getName();
}

bool WorldState::tileDrawnAt(const ska::Tile& tile, const long x, const long y) {
	if (m_tilesetPixelsName != getTilesetName()) {
		m_tilesetPixels.load32(getTilesetName() + ".png");
		m_tilesetPixelsName = getTilesetName();
	}
	const auto size = static_cast<long>(m_tileset->getTileSize());
	const auto* surface = m_tilesetPixels.getInstance();
	const auto pixelX = tile.id.x * size + x;
	const auto pixelY = tile.id.y * size + y;
	if (surface == nullptr || pixelX < 0 || pixelY < 0 || pixelX >= surface->w || pixelY >= surface->h) {
		return true;
	}
	const auto color = m_tilesetPixels.getPixel32Color(static_cast<int>(pixelX), static_cast<int>(pixelY));
	return color.a != 0 && !(color.r == ska::Texture::DEFAULT_T_RED && color.g == ska::Texture::DEFAULT_T_GREEN && color.b == ska::Texture::DEFAULT_T_BLUE);
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
	// The world is drawn with the zoom of the camera (ex : the battles), the interface (GUI) is not
	drawables.setWorldZoom(m_cameraSystem->getZoom(), WorldInterfacePriority);
	m_fade.setScreenSize(m_screenSize);
	drawables.add(m_fade);


	/* Hello, world */
	m_world.graphicUpdate(m_cameraSystem->getDisplay(), drawables);
	//TODO enlever la météo de world
	m_world.getWeather().graphicUpdate(m_cameraSystem->getDisplay(), drawables);
	m_world.getFog().graphicUpdate(m_cameraSystem->getDisplay(), drawables);
	/*
	const auto* trainer = m_entityLocator.getEntityId(SCRIPT_ENTITY_TRAINER);
	{
		if (trainer != nullptr) {
			m_posHero = m_entityManager.getComponent<ska::PositionComponent>(*trainer);
			//m_posHero = m_debugDrawer.getPosition();
			
			m_posHeroPolygon.setOffset(ska::Point<int> { static_cast<int>(m_posHero.x - m_cameraSystem->getDisplay().x), static_cast<int>(m_posHero.y - m_cameraSystem->getDisplay().y)});
			m_posHeroPolygon.setPriority(std::numeric_limits<int>::max());
			drawables.add(m_posHeroPolygon);
		}
	}*/
	
#ifdef PKMN_DRAW_DBG_SHAPES
#ifdef PKMN_DRAW_CONTROL_BODY
	if (m_spaceSystem == nullptr) return;
	const auto& hc = m_entityManager.getComponent < ska::cp::HitboxComponent>(*trainer);
	const auto& controlBody = m_spaceSystem->getSpace().getBody(hc.controlBodyIndex);

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
	m_fade.update(timeStep);
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
	// The characters in the air are drawn over the tree tops, the hops stay on the ground (Utils/IDs.h)
	graphicSystem->setAirLayer(AirHeight, AirPriority);
	// A character whose feet are under the drawing of tiles of the top layer of the map (ex : behind a tree, under its
	// tip ; pushed into its foliage by a hit) is sorted behind all the tiles over its feet (up to the first block without
	// one) ; beside them (on another block, by the transparent corner of the tip), it is drawn over them
	graphicSystem->setSpriteDepth([this](const long x, const long bottom) {
		const auto& collisions = getWorld().getCollisionProfile();
		const auto blockSize = static_cast<long>(collisions.getBlockSize());
		const auto feet = bottom - CharacterFeetMargin;
		if (collisions.layers() == 0 || feet < 0 || blockSize == 0) {
			return 0;
		}
		const auto top = collisions.layers() - 1;
		// A tile of the top layer is drawn at this point of the map
		const auto drawnAt = [&](const long pointX) {
			if (pointX < 0) {
				return false;
			}
			const auto* tile = collisions.getBlock(top, static_cast<std::size_t>(pointX / blockSize), static_cast<std::size_t>(feet / blockSize));
			return tile != nullptr && tileDrawnAt(*tile, pointX % blockSize, feet % blockSize);
		};
		// The whole width of the feet : half of them behind the edge of a tree, the character is beside it
		if (!drawnAt(x) || !drawnAt(x - CharacterFeetHalfWidth) || !drawnAt(x + CharacterFeetHalfWidth)) {
			return 0;
		}
		const auto column = static_cast<std::size_t>(x / blockSize);
		auto row = feet / blockSize;
		while (row > 0 && collisions.getBlock(top, column, static_cast<std::size_t>(row - 1)) != nullptr) {
			row--;
		}
		// Sorted as a character whose sprite ends at the top of the highest of these tiles (above their depth line)
		return static_cast<int>(row * blockSize - bottom);
	});
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
	addLogic(std::make_unique<ska::ZGravityForceSystem>(m_entityManager, 0.90F));
	auto animSystemPtr = std::make_unique<PokemonAnimationSystem>(m_entityManager);
	auto& animSystem = *animSystemPtr.get();
	addLogic(std::move(animSystemPtr));

	m_walkASM = &animSystem.setup(true, std::make_unique<ska::WalkAnimationStateMachine>(m_entityManager));

	animSystem.setup<ska::JumpAnimationStateMachine>(false, std::make_unique<ska::JumpAnimationStateMachine>(m_entityManager));

	animSystem.link<ska::WalkAnimationStateMachine, ska::JumpAnimationStateMachine>([&](const ska::EntityId& e) {
		const auto& pc = m_entityManager.getComponent<ska::PositionComponent>(e);
		return pc.z > 1;
	});

	animSystem.link<ska::JumpAnimationStateMachine, ska::WalkAnimationStateMachine>([&](const ska::EntityId& e) {
		const auto& pc = m_entityManager.getComponent<ska::PositionComponent>(e);
		return pc.z <= 1;
	});

	SettingsChangeEvent sce(SettingsChangeEventType::ALL, m_settings);
	m_eventDispatcher.ska::Observable<SettingsChangeEvent>::notifyObservers(sce);

	m_collisionEventSender = std::make_unique<ska::cp::SpaceCollisionEventSender>(m_entityManager, m_spaceSystem->getSpace(), m_eventDispatcher, m_tileset->getTileSize());
}

void WorldState::afterLoad(ska::State* lastState) {
	m_saveManager.loadGame("save1");
	m_bag.load(m_saveManager.getPathName());
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
			// Not in a wall, nor on the blocks excluded by the data (on_blockid_<i>)
			const auto bX = dest.x / blockSize;
			const auto bY = dest.y / blockSize;
			auto spawnAllowed = bX < m_world.getBlocksX() && bY < m_world.getBlocksY() && !m_world.getCollisionProfile().collide(1, bX, bY);
			if (spawnAllowed) {
				const auto* block = m_world.getCollisionProfile().getBlock(0, bX, bY);
				for (const auto idBlock : idBlocks) {
					if (block != nullptr && static_cast<unsigned int>(block->id.x + block->id.y * blockSize) == idBlock) {
						spawnAllowed = false;
					}
				}
			}

			if (spawnAllowed) {
				// Between the minimum and the maximum level of the area
				const auto levelMin = dataSpawn->get<int>("Data level_min");
				const auto levelMax = std::max(levelMin, dataSpawn->get<int>("Data level_max"));
				auto level = rand() % (levelMax - levelMin + 1) + levelMin;
				auto mob = CustomEntityManager::createMovingCharacterNG(m_entityManager, getSpace(), ska::Point<int>(dest.x / blockSize, dest.y / blockSize), idMob, blockSize);
				/* 0 = Predifined */
				/* 1 = Random */
				/* 2 = Fixe */
				auto type = dataSpawn->get<int>("Data path_type");
				// Random : it wanders block by block, as the characters (GridMovementSystem). Predefined paths are not
				// written in the data : it stays
				if (type == 1) {
					auto grid = GridMovementComponent{};
					grid.mode = GridMovementComponent::Mode::Wander;
					grid.wait = static_cast<unsigned int>(500 + rand() % 1500);
					m_entityManager.addComponent<GridMovementComponent>(mob, std::move(grid));
				}

				FightComponent fc;
				fc.level = level;
				fc.opponentScriptId = idMob;
				m_entityManager.addComponent<FightComponent>(mob, std::move(fc));

				auto& pc = m_entityManager.getComponent<ska::PositionComponent>(mob);
				const auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(mob);
				ska::Rectangle hitbox{ static_cast<int>(pc.x + hc.xOffset), static_cast<int>(pc.y + hc.yOffset), static_cast<int>(hc.width), static_cast<int>(hc.height) };

				const auto targetBlock = m_world.getCollisionProfile().placeOnNearestPracticableBlock(1, hitbox, 1);
				// Exactly on the block (the position of a character is the corner of its block : drawn and sorted with the
				// rows of the map from there)
				const auto spawnBlock = ska::Point<int>{ targetBlock.x / static_cast<int>(blockSize), targetBlock.y / static_cast<int>(blockSize) };
				CustomEntityManager::place(m_entityManager, getSpace(), mob, ska::Point<float>(static_cast<float>(spawnBlock.x * static_cast<int>(blockSize)), static_cast<float>(spawnBlock.y * static_cast<int>(blockSize))));

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
		// The map the player is on (the return from a battle, the saves)
		m_worldFileName = fileName;
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
	hitboxHero.push_back({ hcHero.xOffset, hcHero.yOffset, static_cast<int>(hcHero.width), static_cast<int>(hcHero.height) });
	m_posHeroPolygon = ska::Polygon<int>{ std::move(hitboxHero) };

	//DEBUT Tile map
	// The walls are filled (nothing can stand inside a wall, ex : after leaving the water) ; the water is only its outline :
	// the player surfs inside it, and touching the outline starts or stops surfing (Levels/surf.miniska)
	const auto isWater = [](const ska::Tile* b) {
		return b != nullptr && (b->properties.bitMask & ska::TilePropertiesType::TILE_PROPERTY_WATER) == ska::TilePropertiesType::TILE_PROPERTY_WATER;
	};
	const auto contourRectangleTile = GenerateFilledTileMap(1, m_world.getCollisionProfile(), [&isWater](const ska::Tile* b) {
		if (b == nullptr || isWater(b)) {
			return ska::TileCollision::No;
		}
		return b->collision;
	});

	const auto agglomeratedTilesWater = GenerateAgglomeratedTileMap(1, m_world.getCollisionProfile(), [&isWater](const ska::Tile* b) {
		return isWater(b) ? ska::TileCollision::Yes : ska::TileCollision::No;
	});
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
	const auto wet = m_firstState ? ska::WorldEventType::WORLD_CREATE : ska::WorldEventType::WORLD_CHANGE;
	// With the music of the world : the sound module plays it
	auto we = ska::WorldEvent{ wet, m_worldBGM };
	we.blocksWidth = m_world.getBlocksX();
	we.blocksHeight = m_world.getBlocksY();
	we.blockSize = m_world.getBlockSize();
	m_eventDispatcher.ska::Observable<ska::WorldEvent>::notifyObservers(we);


	//Chargement des NPC sur la map (personnages & pokémon)
}

ska::cp::Space& WorldState::getSpace() {
	assert(m_spaceSystem != nullptr && "Bad instantiation of WorldState : state not loaded but trying to access its members");
	return m_spaceSystem->getSpace();
}

ska::ScriptRunner& WorldState::getScriptRunner() {
	return *m_scripts;
}

ska::scripting::ScriptVM& WorldState::getScripts() {
	return *m_scripts;
}

BattleRules& WorldState::getBattleRules() {
	return *m_battleRules;
}

void WorldState::emitPlayerHook(const std::string& event) {
	if (m_scripts->hooks(event) == 0) {
		return;
	}
	const auto* trainer = m_entityLocator.getEntityId(SCRIPT_ENTITY_TRAINER);
	if (trainer != nullptr) {
		m_engineScriptModule->emit(event, *trainer);
	}
}

void WorldState::battleEnded(const BattleResult& result) {
	const auto won = result.won;
	m_battles++;
	m_lastBattleWon = won;
	m_lastBattleFled = result.fled;
	// The team keeps the hit points left and the experience won
	if (result.teamIndex >= 0) {
		auto team = Team{ m_saveManager.getPathName() };
		auto pokemon = team.member(result.teamIndex);
		if (pokemon.has_value()) {
			pokemon->hp = result.hpLeft;
			pokemon->experience += result.experience;
			team.setMember(result.teamIndex, *pokemon);
			SLOG(ska::LogLevel::Info) << "Battle : the pokemon " << pokemon->id << " of the team has " << pokemon->hp << " hit points, " << pokemon->experience << " experience (+" << result.experience << ")";
		}
	}
	const auto* trainer = m_entityLocator.getEntityId(SCRIPT_ENTITY_TRAINER);
	m_engineScriptModule->emit(won ? "battle.won" : (result.fled ? "battle.fled" : "battle.lost"), trainer == nullptr ? std::optional<ska::EntityId>{} : std::optional<ska::EntityId>{ *trainer });
}

WorldState::~WorldState() = default;

SavegameManager& WorldState::getSaveGame() {
	return m_saveManager;
}
