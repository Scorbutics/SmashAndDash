#include "Game/__internalConfig/LoggerConfig.h"
#include "Game/Gameplay/Data/SavePaths.h"
#include "Graphic/System/GraphicSystem.h"
#include "Graphic/Texture.h"
#include "Graphic/System/ShadowSystem.h"
#include "Graphic/System/AnimationSystem.h"
#include "Graphic/System/DeleterSystem.h"
#include "../Control/ExploreCameraStrategy.h"
#include "../Control/GridMovementComponent.h"
#include "../../Graphic/CharsetSheet.h"
#include "Core/Utils/TimeUtils.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Core/Utils/RectangleUtils.h"
#include "Core/Rectangle.h"
#include "Base/IO/Readers/IniReader.h"
#include "AI/IARandomMovementComponent.h"
#include "AI/IADefinedMovementComponent.h"
#include "WorldState.h"
#include "../Control/GridMovementComponent.h"
#include "../Mobs/WildPokemonComponent.h"
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
#include "Base/IO/Files/FileUtils.h"
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
#include "../Mobs/ArrivalComponent.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"

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
	m_saveManager(em, ed, DefaultSaveName()),
	m_bag(ed),
	m_worldBGM(DEFAULT_BGM), 
	m_eventDispatcher(ed),
	m_entityManager(em),
	m_entityLocator(ed),
	m_entityHandles(ed),
	m_correspondanceMapper("Resources/Chipsets/corr.png"),
	// The tileset of the start map of the save (its world is known once the save is loaded)
	m_tileset(BuildTileset(48, BuildTilesetLoader(TilesetName(m_saveManager.getStartChipsetName())), BuildTilesetEventLoader(TilesetName(m_saveManager.getStartChipsetName())))),
	m_worldFileName(MapDirectory(m_saveManager.getStartMapName())),
	m_world(ed, *m_tileset, BuildWorldLoader(m_correspondanceMapper, m_worldFileName)),
	m_debugDrawer(ed, em, m_entityLocator, m_tileset->getTileSize()) {

	m_scripts = std::make_unique<ska::scripting::ScriptVM>();
	m_engineScriptModule = std::make_unique<ska::scripting::EngineModule>(*m_scripts, m_entityManager, m_entityLocator, m_entityHandles, &m_saveManager);
	// Scripts can hide a character (Engine.disableComponent(character, "GraphicComponent"))
	m_engineScriptModule->registerComponent<ska::GraphicComponent>("GraphicComponent");
	m_gameScriptModules = std::make_unique<GameScriptModules>(*m_scripts, m_entityManager, m_eventDispatcher, *this);
	// The code sent to the development console (tools/ska-console eval) sees the game modules, under their usual names
	for (const auto* prelude : { "GameDialog = import \"bind:game.dialog\"", "Character = import \"bind:game.character\"",
		"Map = import \"bind:game.map\"", "Party = import \"bind:game.party\"", "Interface = import \"bind:game.interface\"",
		"Battle = import \"bind:game.battle\"", "Save = import \"bind:game.save\"", "Bag = import \"bind:game.inventory\"" }) {
		m_scripts->addConsolePrelude(prelude);
	}
	m_battleRules = std::make_unique<BattleRules>(*m_scripts);
	m_scripts->declareEvent("battle.won");
	m_scripts->declareEvent("battle.lost");
	m_scripts->declareEvent("battle.fled");
	m_scripts->declareEvent(InputHooks::Action);
	m_scripts->declareEvent(InputHooks::Jump);

	ska::IniReader reader(SaveDirectory() + m_saveManager.getPathName() + "/trainer.ini");

	m_posHero.x = reader.get<int>("Trainer start_posx");
	m_posHero.y = reader.get<int>("Trainer start_posy");
	auto startMapName = reader.get<std::string>("Trainer start_map_name");

	ska::IniReader mapReader(MapIni(startMapName));
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

float WorldState::getSoundVolume() const {
	return m_settings.getSoundVolume();
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
	// The world is drawn with the zoom of the camera (ex : the battles), the interface (GUI) is not
	drawables.setWorldZoom(m_cameraSystem->getZoom(), WorldInterfacePriority);
	m_fade.setScreenSize(m_screenSize);
	drawables.add(m_fade);


	/* Hello, world */
	m_world.graphicUpdate(m_cameraSystem->getDisplay(), drawables);
	m_lightMap.graphicUpdate(m_cameraSystem->getDisplay(), drawables);
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
	// The wild Pokemon asked by the scripts (Battle.spawnWild), out of their run
	auto wild = std::move(m_wildToSpawn);
	m_wildToSpawn.clear();
	for (const auto& [monster, level] : wild) {
		spawnWildNow(monster, level);
	}
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

	auto cameraSystem = std::make_unique<ska::CameraSystem>(m_entityManager, m_eventDispatcher, std::make_unique<ExploreCameraStrategy>(m_entityManager), m_screenSize.x, m_screenSize.y);
	m_cameraSystem = cameraSystem.get();
	addLogic(std::move(cameraSystem));

	auto graphicSystem = std::make_unique<ska::GraphicSystem>(m_entityManager, m_eventDispatcher, *m_cameraSystem);
	m_graphicSystem = graphicSystem.get();
	m_debugDrawer.setGraphicSystem(m_graphicSystem);
	// The characters in the air are drawn over the tree tops, the hops stay on the ground (Utils/IDs.h)
	graphicSystem->setAirLayer(AirHeight, AirPriority);
	graphicSystem->setSilhouettePriority(SilhouettePriority);
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
	animSystem.setup<ska::RunAnimationStateMachine>(false, std::make_unique<ska::RunAnimationStateMachine>(m_entityManager, Charset::RunColumn));
	animSystem.setup<ska::PoseAnimationStateMachine>(false, std::make_unique<ska::PoseAnimationStateMachine>(m_entityManager));

	// In the air : the jump ; on the ground, the player walking freely (GridMovementComponent::gait, a sheet with the run :
	// Charset) runs, and shows a pose between its gaits (setting off, pushing off, braking) ; the others walk
	const auto inAir = [this](const ska::EntityId& e) {
		return m_entityManager.getComponent<ska::PositionComponent>(e).z > 1;
	};
	const auto gaits = [this, inAir](const ska::EntityId& e) -> const GridMovementComponent* {
		if (inAir(e) || !m_entityManager.hasComponent<GridMovementComponent>(e)) {
			return nullptr;
		}
		const auto& grid = m_entityManager.getComponent<GridMovementComponent>(e);
		const auto& animation = m_entityManager.getComponent<ska::AnimationComponent>(e);
		const auto& graphic = m_entityManager.getComponent<ska::GraphicComponent>(e);
		// Moved by its keys right now (not a script, not a battle)
		const auto driven = animation.drivenUntil > ska::TimeUtils::getTicks();
		return grid.mode == GridMovementComponent::Mode::Keys && driven && !graphic.animatedSprites.empty() && Charset::hasGaits(graphic.animatedSprites[0]) ? &grid : nullptr;
	};
	const auto posing = [gaits](const ska::EntityId& e) {
		const auto* grid = gaits(e);
		return grid != nullptr && grid->poseUntil > ska::TimeUtils::getTicks();
	};
	const auto running = [gaits, posing](const ska::EntityId& e) {
		const auto* grid = gaits(e);
		return grid != nullptr && grid->gait == 2 && !posing(e);
	};
	const auto walking = [inAir, posing, running](const ska::EntityId& e) {
		return !inAir(e) && !posing(e) && !running(e);
	};

	animSystem.link<ska::WalkAnimationStateMachine, ska::JumpAnimationStateMachine>(inAir);
	animSystem.link<ska::WalkAnimationStateMachine, ska::RunAnimationStateMachine>(running);
	animSystem.link<ska::WalkAnimationStateMachine, ska::PoseAnimationStateMachine>(posing);
	animSystem.link<ska::RunAnimationStateMachine, ska::WalkAnimationStateMachine>(walking);
	animSystem.link<ska::RunAnimationStateMachine, ska::JumpAnimationStateMachine>(inAir);
	animSystem.link<ska::RunAnimationStateMachine, ska::PoseAnimationStateMachine>(posing);
	animSystem.link<ska::PoseAnimationStateMachine, ska::WalkAnimationStateMachine>(walking);
	animSystem.link<ska::PoseAnimationStateMachine, ska::RunAnimationStateMachine>(running);
	animSystem.link<ska::PoseAnimationStateMachine, ska::JumpAnimationStateMachine>(inAir);
	animSystem.link<ska::JumpAnimationStateMachine, ska::WalkAnimationStateMachine>([inAir](const ska::EntityId& e) {
		return !inAir(e);
	});

	SettingsChangeEvent sce(SettingsChangeEventType::ALL, m_settings);
	m_eventDispatcher.ska::Observable<SettingsChangeEvent>::notifyObservers(sce);

	m_collisionEventSender = std::make_unique<ska::cp::SpaceCollisionEventSender>(m_entityManager, m_spaceSystem->getSpace(), m_eventDispatcher, m_tileset->getTileSize());
}

void WorldState::afterLoad(ska::State* lastState) {
	m_saveManager.loadGame(DefaultSaveName());
	m_bag.load(m_saveManager.getPathName());
}

void WorldState::beforeUnload() {
	m_worldBGM.setVolume(m_settings.getSoundVolume());
}

int WorldState::teamLevel() {
	const auto team = Team{ m_saveManager.getPathName() };
	const auto index = team.firstAble();
	if (!index.has_value()) {
		return 1;
	}
	const auto pokemon = team.member(*index);
	if (!pokemon.has_value()) {
		return 1;
	}
	const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(pokemon->id) + ".ini" };
	const auto needed = data.exists("Experience level_1_exp_needed") ? data.get<int>("Experience level_1_exp_needed") : 0;
	return m_battleRules->level(pokemon->experience, needed);
}

bool WorldState::freeBlock(const int x, const int y) {
	if (x < 0 || y < 0 || static_cast<std::size_t>(x) >= m_world.getBlocksX() || static_cast<std::size_t>(y) >= m_world.getBlocksY()) {
		return false;
	}
	const auto& collisions = m_world.getCollisionProfile();
	if (collisions.collide(1, static_cast<std::size_t>(x), static_cast<std::size_t>(y))) {
		return false;
	}
	const auto* ground = collisions.getBlock(0, static_cast<std::size_t>(x), static_cast<std::size_t>(y));
	if (ground != nullptr && (ground->properties.bitMask & ska::TilePropertiesType::TILE_PROPERTY_WATER) != 0) {
		return false;
	}
	// Nobody there (a character, another wild Pokemon), no solid part of a tile : where a character stands on the block (the
	// middle of its lower half : ska CollisionProfile::StandCells)
	const auto blockSize = static_cast<double>(m_world.getBlockSize());
	const auto margin = 2.;
	return !getSpace().shapeIn(cpBBNew(x * blockSize + blockSize / 4. + margin, y * blockSize + blockSize / 2. + margin,
		x * blockSize + 3. * blockSize / 4. - margin, (y + 1) * blockSize - margin));
}

ska::EntityId WorldState::createWild(const int monster, const int level, const int pathType, const std::string& attitude, const ska::Point<int> block, const std::optional<int> stay) {
	const auto blockSize = m_world.getBlockSize();
	auto mob = CustomEntityManager::createMovingCharacterNG(m_entityManager, getSpace(), block, monster, blockSize);
	auto wild = WildPokemonComponent{};
	wild.attitude = WildPokemonComponent::FromName(attitude);
	wild.stay = stay;
	// Random : it wanders block by block, as the characters (GridMovementSystem). Predefined paths are not written in the
	// data : it stays, unless it charges the player or runs away from it
	if (pathType == 1 || wild.attitude != WildPokemonComponent::Attitude::Passive) {
		auto grid = GridMovementComponent{};
		grid.mode = GridMovementComponent::Mode::Wander;
		grid.wait = static_cast<unsigned int>(500 + rand() % 1500);
		const auto* trainer = m_entityLocator.getEntityId(SCRIPT_ENTITY_TRAINER);
		ReactToPlayer(grid, wild.attitude, trainer == nullptr ? 0 : *trainer);
		m_entityManager.addComponent<GridMovementComponent>(mob, std::move(grid));
	}
	m_entityManager.addComponent<WildPokemonComponent>(mob, std::move(wild));

	FightComponent fc;
	fc.level = level;
	fc.opponentScriptId = monster;
	m_entityManager.addComponent<FightComponent>(mob, std::move(fc));
	// Exactly on the block (the position of a character is the corner of its block)
	CustomEntityManager::place(m_entityManager, getSpace(), mob, ska::Point<float>(static_cast<float>(block.x * static_cast<int>(blockSize)), static_cast<float>(block.y * static_cast<int>(blockSize))));
	SLOG(ska::LogLevel::Info) << "Wild pokemon " << monster << " level " << level << " at block (" << block.x << ", " << block.y << "), " << attitude;
	return mob;
}

ska::EntityId WorldState::arrivalDrawing(const int monster, ArrivalComponent arrival) {
	// The character without its physics body (it flies over the walls) : its drawing, its shadow, its feet (the hitbox, as a
	// character with a body : CustomEntityManager)
	const auto blockSize = m_world.getBlockSize();
	auto entity = m_entityManager.createCharacter(ska::Point<int>{ 0, 0 }, monster, blockSize);
	m_entityManager.removeComponent<ska::AnimationComponent>(entity);
	auto& hitbox = m_entityManager.getComponent<ska::HitboxComponent>(entity);
	hitbox.yOffset -= static_cast<int>(hitbox.height);
	auto& position = m_entityManager.getComponent<ska::PositionComponent>(entity);
	position.x = static_cast<long>(arrival.from.x);
	position.y = static_cast<long>(arrival.from.y);
	position.z = static_cast<long>(arrival.fromHeight);
	m_entityManager.addComponent<ArrivalComponent>(entity, std::move(arrival));
	return entity;
}

void WorldState::wildArrived(const ArrivalComponent& arrival) {
	const auto blockSize = static_cast<int>(m_world.getBlockSize());
	auto block = ska::Point<int>{ static_cast<int>(std::lround(arrival.to.x)) / blockSize, static_cast<int>(std::lround(arrival.to.y)) / blockSize };
	// Someone came there meanwhile : the nearest free block
	if (!freeBlock(block.x, block.y)) {
		const auto& hc = ska::Rectangle{ block.x * blockSize, block.y * blockSize, blockSize, blockSize };
		const auto target = m_world.getCollisionProfile().placeOnNearestPracticableBlock(1, hc, 2);
		block = { target.x / blockSize, target.y / blockSize };
	}
	createWild(arrival.monster, arrival.level, arrival.pathType, arrival.attitude, block, arrival.stay);
}

void WorldState::takeOff(const ska::EntityId wild) {
	if (!m_entityManager.hasComponent<FightComponent>(wild) || !m_entityManager.hasComponent<ska::PositionComponent>(wild)) {
		return;
	}
	// Copies : the drawing made below moves the components
	const auto monster = static_cast<int>(m_entityManager.getComponent<FightComponent>(wild).opponentScriptId);
	const auto position = m_entityManager.getComponent<ska::PositionComponent>(wild);
	const auto blockSize = static_cast<float>(m_world.getBlockSize());
	// Away from the player, out of the screen
	auto away = ska::Point<float>{ 0.F, -1.F };
	const auto* trainer = m_entityLocator.getEntityId(SCRIPT_ENTITY_TRAINER);
	if (trainer != nullptr && m_entityManager.hasComponent<ska::PositionComponent>(*trainer)) {
		const auto& player = m_entityManager.getComponent<ska::PositionComponent>(*trainer);
		const auto dx = static_cast<float>(position.x - player.x);
		const auto dy = static_cast<float>(position.y - player.y);
		const auto length = std::hypot(dx, dy);
		if (length > 1.F) {
			away = { dx / length, dy / length };
		}
	}
	auto arrival = ArrivalComponent{};
	arrival.kind = ArrivalComponent::Kind::TakeOff;
	arrival.from = { static_cast<float>(position.x), static_cast<float>(position.y) };
	arrival.to = { arrival.from.x + away.x * FlightDistance * blockSize, arrival.from.y + away.y * FlightDistance * blockSize };
	arrival.fromHeight = 0.F;
	arrival.toHeight = static_cast<float>(FlightHeight);
	arrival.duration = 2200;
	SLOG(ska::LogLevel::Info) << "Wild pokemon " << monster << " flies away from block (" << position.x / static_cast<long>(blockSize) << ", " << position.y / static_cast<long>(blockSize) << ")";
	arrivalDrawing(monster, std::move(arrival));
	// The wild Pokemon leaves : its body parked out of the physics space (it keeps it), then removed
	CustomEntityManager::place(m_entityManager, getSpace(), wild, ska::Point<float>{ -100000.F, -100000.F });
	m_entityManager.removeEntity(wild);
}

std::optional<ska::Point<float>> WorldState::playerPosition() {
	const auto* trainer = m_entityLocator.getEntityId(SCRIPT_ENTITY_TRAINER);
	if (trainer == nullptr || !m_entityManager.hasComponent<ska::PositionComponent>(*trainer)) {
		return std::nullopt;
	}
	const auto& position = m_entityManager.getComponent<ska::PositionComponent>(*trainer);
	return ska::Point<float>{ static_cast<float>(position.x), static_cast<float>(position.y) };
}

bool WorldState::land(const int monster, const int level, const std::string& attitude, const ska::Point<int> block, const int stay) {
	const auto player = playerPosition();
	if (!player.has_value()) {
		return false;
	}
	const auto blockSize = static_cast<float>(m_world.getBlockSize());
	auto flight = ArrivalComponent{};
	flight.kind = ArrivalComponent::Kind::Land;
	flight.to = { static_cast<float>(block.x) * blockSize, static_cast<float>(block.y) * blockSize };
	// From out of the screen, high in the sky : beyond its block, seen from the player
	auto away = ska::Point<float>{ flight.to.x - player->x, flight.to.y - player->y };
	const auto length = std::hypot(away.x, away.y);
	away = length > 1.F ? ska::Point<float>{ away.x / length, away.y / length } : ska::Point<float>{ 0.F, -1.F };
	flight.from = { flight.to.x + away.x * FlightDistance * blockSize, flight.to.y + away.y * FlightDistance * blockSize };
	flight.fromHeight = static_cast<float>(FlightHeight);
	flight.duration = 3400;
	flight.monster = monster;
	flight.level = level;
	flight.pathType = 1;
	flight.attitude = attitude;
	flight.stay = stay > 0 ? std::optional<int>{ stay } : std::nullopt;
	arrivalDrawing(monster, std::move(flight));
	SLOG(ska::LogLevel::Info) << "Wild pokemon " << monster << " flies in to block (" << block.x << ", " << block.y << ")";
	return true;
}

bool WorldState::emerge(const int monster, const int level, const std::string& attitude, const ska::Point<int> hideout, const ska::Point<int> block, const int rustle, const std::string& effect) {
	if (!playerPosition().has_value()) {
		return false;
	}
	const auto blockSize = static_cast<float>(m_world.getBlockSize());
	auto emerge = ArrivalComponent{};
	emerge.kind = ArrivalComponent::Kind::Emerge;
	emerge.from = { static_cast<float>(hideout.x) * blockSize, static_cast<float>(hideout.y) * blockSize };
	emerge.to = { static_cast<float>(block.x) * blockSize, static_cast<float>(block.y) * blockSize };
	emerge.arc = 30.F;
	emerge.wait = static_cast<unsigned int>(std::max(0, rustle));
	emerge.duration = 420;
	emerge.monster = monster;
	emerge.level = level;
	emerge.pathType = 1;
	emerge.attitude = attitude;
	emerge.hideout = hideout;
	emerge.effect = effect == "splash" ? ArrivalComponent::Effect::Splash : (effect == "dust" ? ArrivalComponent::Effect::Dust : ArrivalComponent::Effect::Leaves);
	arrivalDrawing(monster, std::move(emerge));
	SLOG(ska::LogLevel::Info) << "Wild pokemon " << monster << " comes out of the hideout at block (" << hideout.x << ", " << hideout.y << ") : " << effect;
	return true;
}

bool WorldState::drop(const int monster, const int level, const std::string& attitude, const ska::Point<int> leaves, const ska::Point<int> block, const int rustle) {
	if (!playerPosition().has_value() || block.y <= leaves.y) {
		return false;
	}
	const auto blockSize = static_cast<float>(m_world.getBlockSize());
	auto drop = ArrivalComponent{};
	drop.kind = ArrivalComponent::Kind::Drop;
	drop.to = { static_cast<float>(block.x) * blockSize, static_cast<float>(block.y) * blockSize };
	// It faces down (its path : from the block above)
	drop.from = { drop.to.x, drop.to.y - blockSize };
	// From the height of the leaves over its block
	drop.fromHeight = static_cast<float>(block.y - leaves.y) * blockSize - 8.F;
	drop.wait = static_cast<unsigned int>(std::max(0, rustle));
	drop.duration = 1300;
	drop.monster = monster;
	drop.level = level;
	drop.pathType = 1;
	drop.attitude = attitude;
	drop.hideout = leaves;
	drop.effect = ArrivalComponent::Effect::Leaves;
	arrivalDrawing(monster, std::move(drop));
	SLOG(ska::LogLevel::Info) << "Wild pokemon " << monster << " comes down from the leaves at block (" << leaves.x << ", " << leaves.y << ")";
	return true;
}

bool WorldState::walkIn(const int monster, const int level, const std::string& attitude) {
	const auto player = playerPosition();
	if (!player.has_value()) {
		return false;
	}
	const auto blockSize = m_world.getBlockSize();
	// A few tries around the player (a block may be a wall)
	for (auto i = 0; i < 12; i++) {
		const auto angle = static_cast<float>((2 * M_PI * (rand() % 360)) / 360);
		if (walkInAt(monster, level, 1, attitude, { static_cast<int>(player->x), static_cast<int>(player->y) }, angle, 8 * blockSize, 13 * blockSize, {})) {
			return true;
		}
	}
	SLOG(ska::LogLevel::Info) << "Wild pokemon " << monster << " : no room for it near the player";
	return false;
}

bool WorldState::spawnOne(const int idMob, const int level, const int pathType, const ska::Point<int> near, const float angle, const unsigned int rmin, const unsigned int rmax, const std::vector<unsigned int>& idBlocks) {
	const auto blockSize = static_cast<int>(m_world.getBlockSize());
	// How it arrives (Data/Battle/encounters.miniska) ; its attitude towards the player
	const auto arrival = m_battleRules->arrival(idMob);
	const auto attitude = m_battleRules->attitude(idMob, level, teamLevel(), rand() % 100);
	if (arrival != "walk") {
		// It flies in, it comes out of a hideout : the script places it (Data/Battle/arrivals.miniska), the dice rolled here
		const auto playerBlock = ska::Point<int>{ (near.x + blockSize / 2) / blockSize, (near.y + blockSize / 2) / blockSize };
		auto context = ska::scripting::ScriptContext{};
		context.name = ArrivalsFile;
		const auto started = getScripts().start(ArrivalsFile, { arrival, std::to_string(idMob), std::to_string(level), attitude,
			std::to_string(playerBlock.x), std::to_string(playerBlock.y), std::to_string(rand() % 10000), std::to_string(rand() % 10000) }, context);
		if (started) {
			return true;
		}
	}
	return walkInAt(idMob, level, pathType, attitude, near, angle, rmin, rmax, idBlocks);
}

bool WorldState::walkInAt(const int idMob, const int level, const int pathType, const std::string& attitude, const ska::Point<int> near, const float angle, const unsigned int rmin, const unsigned int rmax, const std::vector<unsigned int>& idBlocks) {
	const auto blockSize = m_world.getBlockSize();
	// It walks in : out of the screen
	const auto radius = rmin + rand() % (rmax - rmin + 1);
	auto dest = ska::Point<int>{ static_cast<int>(radius * ska::NumberUtils::cosinus(angle) + near.x), static_cast<int>(radius * ska::NumberUtils::sinus(angle) + near.y) };
	dest.x = (dest.x / static_cast<int>(blockSize)) * static_cast<int>(blockSize);
	dest.y = (dest.y / static_cast<int>(blockSize)) * static_cast<int>(blockSize);
	const auto boxWorld = ska::Rectangle{ 0, 0, static_cast<int>(m_world.getPixelWidth()), static_cast<int>(m_world.getPixelHeight()) };
	if (!ska::RectangleUtils::isPositionInBox(dest, boxWorld)) {
		return false;
	}
	// Not in a wall, nor on the blocks excluded by the data (on_blockid_<i>)
	const auto bX = dest.x / static_cast<int>(blockSize);
	const auto bY = dest.y / static_cast<int>(blockSize);
	auto spawnAllowed = static_cast<std::size_t>(bX) < m_world.getBlocksX() && static_cast<std::size_t>(bY) < m_world.getBlocksY() && !m_world.getCollisionProfile().collide(1, bX, bY);
	if (spawnAllowed) {
		const auto* block = m_world.getCollisionProfile().getBlock(0, bX, bY);
		for (const auto idBlock : idBlocks) {
			if (block != nullptr && static_cast<unsigned int>(block->id.x + block->id.y * static_cast<int>(blockSize)) == idBlock) {
				spawnAllowed = false;
			}
		}
	}
	if (!spawnAllowed) {
		SLOG(ska::LogLevel::Debug) << "Wild pokemon " << idMob << " : no spawn at block (" << bX << ", " << bY << ")";
		return false;
	}
	const auto hitbox = ska::Rectangle{ dest.x, dest.y, static_cast<int>(blockSize), static_cast<int>(blockSize) };
	const auto targetBlock = m_world.getCollisionProfile().placeOnNearestPracticableBlock(1, hitbox, 1);
	createWild(idMob, level, pathType, attitude, { targetBlock.x / static_cast<int>(blockSize), targetBlock.y / static_cast<int>(blockSize) }, std::nullopt);
	return true;
}

bool WorldState::spawnWild(const int monster, const int level) {
	const auto* trainer = m_entityLocator.getEntityId(SCRIPT_ENTITY_TRAINER);
	if (trainer == nullptr || !m_entityManager.hasComponent<ska::PositionComponent>(*trainer)) {
		return false;
	}
	// Asked by a script : the rules (scripts too) cannot be called while it runs, the next frame spawns it
	m_wildToSpawn.emplace_back(monster, level);
	return true;
}

bool WorldState::spawnWildNow(const int monster, const int level) {
	const auto player = playerPosition();
	if (!player.has_value()) {
		return false;
	}
	const auto blockSize = m_world.getBlockSize();
	// A few tries around the player (walking in, a block may be a wall)
	for (auto i = 0; i < 12; i++) {
		const auto angle = static_cast<float>((2 * M_PI * (rand() % 360)) / 360);
		if (spawnOne(monster, level, 1, { static_cast<int>(player->x), static_cast<int>(player->y) }, angle, 8 * blockSize, 13 * blockSize, {})) {
			return true;
		}
	}
	SLOG(ska::LogLevel::Info) << "Wild pokemon " << monster << " : no room for it near the player";
	return false;
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
	const auto idMob = dataSpawn->get<int>("Data id");
	/* 0 = Predifined, 1 = Random, 2 = Fixe */
	const auto pathType = dataSpawn->get<int>("Data path_type");
	auto successfulSpawns = 0;
	auto angle = static_cast<float>((2 * M_PI* (rand() % 360)) / 360);
	for (unsigned int i = 0; i < nbrSpawns; i++) {
		// Between the minimum and the maximum level of the area
		const auto levelMin = dataSpawn->get<int>("Data level_min");
		const auto levelMax = std::max(levelMin, dataSpawn->get<int>("Data level_max"));
		const auto level = rand() % (levelMax - levelMin + 1) + levelMin;
		successfulSpawns += spawnOne(idMob, level, pathType, { pos.x, pos.y }, angle, rmin, rmax, idBlocks) ? 1 : 0;
		angle += static_cast<float>((2.0 * M_PI) / nbrSpawns);
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
	// The previous tileset lives until the world has loaded the new one : the world still points to it (it compares
	// the names to know whether the tileset changed)
	auto previousTileset = ska::TilesetPtr{};
	if (getTilesetName() != chipsetName) {
		previousTileset = std::move(m_tileset);
		m_tileset = BuildTileset(previousTileset->getTileSize(), BuildTilesetLoader(chipsetName), BuildTilesetEventLoader(chipsetName));
	}

	// Loaded again even when it is the same map (a teleport on the map the player is on) : its entities were removed
	// above, its loading events (its characters, its item balls) run again. The first map too : the world built with the
	// state did not load its data (wild Pokemon, weather, fog : WorldImpl)
	{
		m_world.load(m_correspondanceMapper, fileName, m_tileset.get());
		m_worldLoaded = true;
		// The map the player is on (the return from a battle, the saves)
		m_worldFileName = fileName;
	}
	// Its light, when it has one : "<world>/<map>" -> "<world>/<map>/<map>L.png"
	const auto mapName = ska::FileNameData{ fileName }.name;
	m_lightMap.setPriority(LightMapPriority);
	m_lightMap.load(fileName + "/" + mapName + "L.png", static_cast<unsigned int>(m_world.getBlockSize()), static_cast<unsigned int>(m_world.getBlocksX()), static_cast<unsigned int>(m_world.getBlocksY()));
	//FIN WORLD

	//DEBUT PLAYER
	auto player = ska::EntityId{};
	if (trainer == nullptr) {
		player = CustomEntityManager::createTrainerNG(m_entityManager, m_spaceSystem->getSpace(), m_posHero, m_world.getBlockSize());
		auto& pShape = m_spaceSystem->getSpace().getShape(m_entityManager.getComponent<ska::cp::HitboxComponent>(player).shapeIndex);
		pShape.setBounciness(0.F);
	} else {
		player = *trainer;
		CustomEntityManager::fillTrainer(m_entityManager, m_spaceSystem->getSpace(), player);
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
