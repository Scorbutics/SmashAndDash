#include "Game/__internalConfig/LoggerConfig.h"
#include "GameScriptModule.h"
#include <limits>
#include <algorithm>
#include <fstream>
#include <optional>
#include "Scripting/ScriptVM.h"
#include "Scripting/EntityHandles.h"
#include "Base/IO/Readers/IniReader.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Physic/CollidableComponent.h"
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "Core/ECS/Basics/Graphic/DeleterComponent.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"
#include "Core/Utils/PhysicUtils.h"
#include "Core/Utils/SpritePath.h"
#include "Core/Utils/SkaConstants.h"
#include "Graphic/GraphicComponent.h"
#include "AI/IADefinedMovementComponent.h"
#include "AI/IAFollowComponent.h"
#include "Core/ECS/Basics/Input/InputComponent.h"
#include "../Gameplay/Control/MovementModeComponent.h"
#include "../Gameplay/Control/GridMovementComponent.h"
#include "Physic/Body.h"
#include "Physic/Vect.h"
#include "../Gameplay/Data/EntityLoadEvent.h"
#include "../Gameplay/Data/SavegameManager.h"
#include "World/TileWorld.h"
#include "World/CollisionProfile.h"
#include "Physic/Space.h"
#include "../Gameplay/CustomEntityManager.h"
#include "../Gameplay/Data/MapEvent.h"
#include "../Gameplay/Data/DialogEvent.h"
#include "../Gameplay/World/WorldState.h"
#include "../Gameplay/Fight/FightComponent.h"
#include "../Gameplay/Fight/BattleRules.h"
#include "../Gameplay/Data/Team.h"
#include "../Gameplay/Data/Bag.h"
#include "../Gameplay/Data/ItemUse.h"
#include "ScriptConstants.h"
#include "../Gameplay/Fight/System/FightStartSystem.h"

namespace {
	// Entity of a script entity handle (an error when the entity was removed)
	ska::EntityId Entity(const ska::scripting::EntityHandles& handles, ska::EntityManager& entityManager, const ska::NodeValue& value) {
		const auto entity = handles.entity(value.nodeval<long>());
		if (!entityManager.hasComponent<ska::PositionComponent>(entity)) {
			throw std::runtime_error("the entity " + std::to_string(value.nodeval<long>()) + " has no position");
		}
		return entity;
	}

	int Int(const ska::NodeValue& value) {
		return static_cast<int>(value.nodeval<long>());
	}

	const std::string& String(const ska::NodeValue& value) {
		return *value.nodeval<ska::StringShared>();
	}

	template <class Component>
	Component& Require(ska::EntityManager& entityManager, ska::EntityId entity, const char* action) {
		if (!entityManager.hasComponent<Component>(entity)) {
			throw std::runtime_error("the entity " + std::to_string(entity) + " cannot " + action);
		}
		return entityManager.getComponent<Component>(entity);
	}

	ska::Point<float> CenterPosition(ska::EntityManager& entityManager, ska::EntityId entity) {
		return ska::PositionComponent::getCenterPosition(entityManager.getComponent<ska::PositionComponent>(entity),
			Require<ska::HitboxComponent>(entityManager, entity, "be located (no hitbox)"));
	}

	// Speed of the "run" movement mode, compared to the walk
	constexpr auto RunFactor = 1.8F;

	// The old scripts separate the lines of a message with a "\xa4"
	std::string MessageText(std::string text) {
		std::replace(text.begin(), text.end(), '\xa4', '\n');
		return text;
	}

	void ShowDialog(PokemonGameEventDispatcher& ged, const std::string& text, bool question, std::function<void(bool)> onClose) {
		// Each dialog has its own window (the previous one can still be scrolling out)
		static auto dialogNumber = 0u;
		auto dialog = DialogEvent{ MessageText(text), "scriptDialog" + std::to_string(dialogNumber++), ska::Rectangle{ 0, std::numeric_limits<int>::max(), -1, 4 * 48 }, true };
		dialog.question = question;
		dialog.onClose = std::move(onClose);
		ged.ska::Observable<DialogEvent>::notifyObservers(dialog);
	}

	// Sets the position of an entity (in pixels) : the position of its physics body, when it has one
	void Place(ska::EntityManager& entityManager, ska::cp::Space& space, ska::EntityId entity, ska::Point<float> position) {
		auto& pc = entityManager.getComponent<ska::PositionComponent>(entity);
		pc.x = static_cast<long>(position.x);
		pc.y = static_cast<long>(position.y);
		if (entityManager.hasComponent<ska::cp::HitboxComponent>(entity)) {
			const auto& hitbox = entityManager.getComponent<ska::cp::HitboxComponent>(entity);
			space.getBody(hitbox.bodyIndex).setPosition(ska::cp::Vect{ position.x, position.y });
			if (hitbox.controlBodyIndex != hitbox.bodyIndex) {
				space.getBody(hitbox.controlBodyIndex).setPosition(ska::cp::Vect{ position.x, position.y });
			}
		}
	}

	// Game features of the old scripts that do not exist in this version of the game : the scripts still call them
	void NotAvailable(const char* feature) {
		SLOG_STATIC(ska::LogLevel::Warn, GameScriptModule) << "Script : \"" << feature << "\" is not available in this version of the game";
	}
}

GameScriptModule::GameScriptModule(ska::scripting::ScriptVM& vm, const std::string& name) :
	ska::lang::Module<ska::bytecode::Interpreter> { vm.modules(), "game." + name, "std:game." + name },
	m_builder(ska::BridgeBuilder<ska::bytecode::Interpreter> { m_bridge }) {
}

DialogScriptModule::DialogScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState) :
	GameScriptModule(vm, "dialog") {
	(void) entityManager;
	(void) worldState;

	m_builder.bindField("message", [&vm, &ged](std::vector<ska::NodeValue> params) {
		auto closed = std::make_shared<bool>(false);
		ShowDialog(ged, String(params[0]), false, [closed](bool) { *closed = true; });
		vm.suspendUntil([closed]() { return *closed; });
		return ska::NodeValue{};
	});

	m_builder.bindField("choice", [&vm, &ged](std::vector<ska::NodeValue> params) {
		auto answer = std::make_shared<std::optional<bool>>();
		ShowDialog(ged, String(params[0]) + "\n\n(Entr\xe9""e : oui, Retour arri\xe8""re : non)", true, [answer](bool validated) { *answer = validated; });
		vm.suspendUntil([answer]() { return answer->has_value(); }, [answer]() { return ska::NodeValue{ answer->value_or(false) }; });
		return ska::NodeValue{};
	});

	m_builder.bindField("cinematic", [&vm, &ged](std::vector<ska::NodeValue> params) {
		// The animation of the cinematic is not played (no cinematic in this version of the game) : only its text is shown
		auto closed = std::make_shared<bool>(false);
		ShowDialog(ged, String(params[1]), false, [closed](bool) { *closed = true; });
		vm.suspendUntil([closed]() { return *closed; });
		return ska::NodeValue{};
	});

	m_builder.generate();
}

CharacterScriptModule::CharacterScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState) :
	GameScriptModule(vm, "character") {
	(void) ged;
	const auto& handles = worldState.getEntityHandles();

	m_builder.bindField("move", [&vm, &entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		const auto direction = Int(params[1]);
		const auto distance = static_cast<float>(Int(params[2]));
		const auto ghost = params[3].nodeval<bool>();

		auto& movement = Require<ska::MovementComponent>(entityManager, entity, "move");
		movement.vx = 0;
		movement.vy = 0;
		movement.ax = 0;
		movement.ay = 0;
		auto& collidable = Require<ska::CollidableComponent>(entityManager, entity, "move (not collidable)");
		const auto wasGhost = collidable.ghost;
		collidable.ghost = ghost;

		auto definedMovement = ska::IADefinedMovementComponent{};
		definedMovement.origin = CenterPosition(entityManager, entity);
		// An entity blocked by another one (not a world collision) gives up after a while : 500 ms per block, at least 1 s
		definedMovement.delay = 1000 + static_cast<unsigned int>(distance / 48.F * 500.F);
		const auto force = ska::PhysicUtils::getMovement(direction, distance);
		definedMovement.directions.push_back(ska::Point<int>::cartesian(static_cast<int>(force.power), force.angle));
		auto done = std::make_shared<bool>(false);
		definedMovement.finished = [done]() { *done = true; };
		entityManager.addComponent<ska::IADefinedMovementComponent>(entity, std::move(definedMovement));

		// The script goes on once the movement is done (or when the movement was removed otherwise : entity removed...)
		vm.suspendUntil([&entityManager, entity, wasGhost, done]() {
			if (!*done && entityManager.hasComponent<ska::IADefinedMovementComponent>(entity)) {
				return false;
			}
			if (entityManager.hasComponent<ska::CollidableComponent>(entity)) {
				entityManager.getComponent<ska::CollidableComponent>(entity).ghost = wasGhost;
			}
			return true;
		});
		return ska::NodeValue{};
	});

	m_builder.bindField("jump", [&entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		Require<ska::MovementComponent>(entityManager, entity, "jump").vz = static_cast<float>(Int(params[1]));
		return ska::NodeValue{};
	});

	m_builder.bindField("addCharacter", [&entityManager, &worldState, &handles](std::vector<ska::NodeValue> params) {
		auto& space = worldState.getSpace();
		const auto& world = worldState.getWorld();
		const auto character = CustomEntityManager::createMovingCharacterNG(entityManager, space, ska::Point<int>{ Int(params[1]), Int(params[2]) },
			Int(params[0]), static_cast<unsigned int>(world.getBlockSize()), String(params[3]));
		const auto& script = String(params[4]);
		if (!script.empty()) {
			auto sleeping = ska::ScriptSleepComponent{};
			sleeping.name = script;
			sleeping.triggeringType = ska::ScriptTriggerType::ACTION;
			sleeping.period = 1000;
			const auto& arguments = params[5].nodeval<ska::NodeValueArray>();
			if (arguments != nullptr) {
				for (const auto& argument : *arguments) {
					sleeping.args.push_back(argument.convertString());
				}
			}
			entityManager.addComponent<ska::ScriptSleepComponent>(character, std::move(sleeping));
		}
		return ska::NodeValue{ handles.handle(character) };
	});

	m_builder.bindField("playAnimation", [&entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		if (!entityManager.hasComponent<ska::HitboxComponent>(entity) || !entityManager.hasComponent<ska::GraphicComponent>(entity)) {
			return ska::NodeValue{};
		}
		const auto& position = entityManager.getComponent<ska::PositionComponent>(entity);
		const auto& hitbox = entityManager.getComponent<ska::HitboxComponent>(entity);
		const auto& graphic = entityManager.getComponent<ska::GraphicComponent>(entity);
		if (graphic.animatedSprites.empty()) {
			return ska::NodeValue{};
		}

		const auto animation = entityManager.createEntity();
		auto animationPosition = ska::PositionComponent{};
		animationPosition.x = position.x;
		animationPosition.y = position.y + hitbox.yOffset - graphic.animatedSprites[0].getHeight();
		entityManager.addComponent<ska::PositionComponent>(animation, std::move(animationPosition));
		auto animationGraphic = ska::GraphicComponent{};
		animationGraphic.animatedSprites.resize(1);
		animationGraphic.animatedSprites[0].load(ska::SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, Int(params[1])), 4, 1, 4);
		animationGraphic.animatedSprites[0].setDelay(300);
		entityManager.addComponent<ska::GraphicComponent>(animation, std::move(animationGraphic));
		auto deleter = ska::DeleterComponent{};
		deleter.delay = 2100;
		entityManager.addComponent<ska::DeleterComponent>(animation, std::move(deleter));
		return ska::NodeValue{};
	});

	m_builder.bindField("setSprite", [&entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		auto& graphic = Require<ska::GraphicComponent>(entityManager, entity, "change its sprite");
		if (graphic.animatedSprites.empty()) {
			graphic.animatedSprites.resize(1);
		}
		// A sheet of the same layout as the characters (6 x 8 frames, 3 animated)
		graphic.animatedSprites[0].load(ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, Int(params[1])), 6, 8, 3);
		graphic.animatedSprites[0].setDelay(100);
		Require<ska::AnimationComponent>(entityManager, entity, "be animated").animatedWhenStill = params[2].nodeval<bool>();
		return ska::NodeValue{};
	});

	m_builder.bindField("direction", [&entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		return ska::NodeValue{ static_cast<long>(Require<ska::AnimationComponent>(entityManager, entity, "have a direction").state) };
	});

	m_builder.bindField("setDirection", [&entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		Require<ska::AnimationComponent>(entityManager, entity, "have a direction").state = params[1].nodeval<long>();
		return ska::NodeValue{};
	});

	m_builder.bindField("setLayer", [&entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		entityManager.getComponent<ska::PositionComponent>(entity).layer = static_cast<std::size_t>(std::max(0, Int(params[1])));
		return ska::NodeValue{};
	});

	m_builder.bindField("place", [&entityManager, &worldState, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		const auto blockSize = static_cast<float>(worldState.getWorld().getBlockSize());
		Place(entityManager, worldState.getSpace(), entity, ska::Point<float>{ Int(params[1]) * blockSize, Int(params[2]) * blockSize });
		return ska::NodeValue{};
	});

	m_builder.bindField("remove", [&entityManager, &worldState, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		// Removing the entity does not remove its physics body : the body is moved away from the map
		Place(entityManager, worldState.getSpace(), entity, ska::Point<float>{ -100000.F, -100000.F });
		entityManager.removeEntity(entity);
		return ska::NodeValue{};
	});

	m_builder.bindField("wander", [&entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		// Block by block : random steps after random waits (Gameplay/Control/GridMovementSystem)
		auto grid = GridMovementComponent{};
		grid.mode = GridMovementComponent::Mode::Wander;
		grid.wait = 1000;
		entityManager.addComponent<GridMovementComponent>(entity, std::move(grid));
		return ska::NodeValue{};
	});

	m_builder.bindField("patrol", [&entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		// Path letters : "b" down, "d" right, "h" up, "g" left
		auto directions = std::vector<int>{};
		for (const auto letter : String(params[1])) {
			const auto direction = std::string{ "bdhg" }.find(letter);
			if (direction == std::string::npos) {
				throw std::runtime_error(std::string{ "unknown path direction \"" } + letter + "\" (b, d, h, g)");
			}
			directions.push_back(static_cast<int>(direction));
		}
		if (directions.empty()) {
			return ska::NodeValue{};
		}
		// Block by block, in a loop : it waits when the way is not free (Gameplay/Control/GridMovementSystem)
		auto grid = GridMovementComponent{};
		grid.mode = GridMovementComponent::Mode::Path;
		grid.path = std::move(directions);
		entityManager.addComponent<GridMovementComponent>(entity, std::move(grid));
		return ska::NodeValue{};
	});

	// Movement modes of a character controlled by the player : the powers of its normal walk are kept
	m_builder.bindField("setMovement", [&entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		const auto& mode = String(params[1]);
		if (mode != "walk" && mode != "run" && mode != "locked") {
			throw std::runtime_error("unknown movement mode \"" + mode + "\" (modes : walk, run, locked)");
		}
		auto& input = Require<ska::InputComponent>(entityManager, entity, "change its movement (not controlled by the player)");
		if (!entityManager.hasComponent<MovementModeComponent>(entity)) {
			auto saved = MovementModeComponent{};
			saved.walkPower = input.movePower;
			saved.jumpPower = input.jumpPower;
			entityManager.addComponent<MovementModeComponent>(entity, std::move(saved));
		}
		auto& movementMode = entityManager.getComponent<MovementModeComponent>(entity);
		movementMode.mode = mode;
		input.movePower = mode == "locked" ? 0.F : (mode == "run" ? movementMode.walkPower * RunFactor : movementMode.walkPower);
		input.jumpPower = mode == "locked" ? 0.F : movementMode.jumpPower;
		return ska::NodeValue{};
	});

	m_builder.bindField("movement", [&entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		const auto mode = entityManager.hasComponent<MovementModeComponent>(entity) ? entityManager.getComponent<MovementModeComponent>(entity).mode : std::string{ "walk" };
		return ska::NodeValue{ std::make_shared<std::string>(mode) };
	});

	m_builder.bindField("follow", [&entityManager, &handles](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(handles, entityManager, params[0]);
		if (params[1].nodeval<long>() < 0) {
			if (entityManager.hasComponent<ska::IAFollowComponent>(entity)) {
				entityManager.removeComponent<ska::IAFollowComponent>(entity);
			}
			return ska::NodeValue{};
		}
		const auto target = Entity(handles, entityManager, params[1]);
		// One block behind its target
		entityManager.addComponent<ska::IAFollowComponent>(entity, ska::IAFollowComponent{ target, 48.F });
		return ska::NodeValue{};
	});

	m_builder.generate();
}

MapScriptModule::MapScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState) :
	GameScriptModule(vm, "map") {
	const auto& handles = worldState.getEntityHandles();

	m_builder.bindField("teleport", [&ged, &worldState](std::vector<ska::NodeValue> params) {
		const auto& world = worldState.getWorld();
		const auto& mapName = String(params[0]);
		const auto mapReader = ska::IniReader{ "./Levels/" + mapName + "/" + mapName + ".ini" };
		auto mapEvent = MapEvent{ MapEventType::Switch_Map };
		mapEvent.chipsetName = mapReader.get<std::string>("Chipset file");
		mapEvent.mapName = mapName;
		const auto blockSize = static_cast<int>(world.getBlockSize());
		mapEvent.position.x = blockSize * Int(params[1]);
		mapEvent.position.y = blockSize * Int(params[2]);
		ged.ska::Observable<MapEvent>::notifyObservers(mapEvent);
		return ska::NodeValue{};
	});

	m_builder.bindField("blockX", [&entityManager, &worldState, &handles](std::vector<ska::NodeValue> params) {
		const auto& world = worldState.getWorld();
		const auto entity = Entity(handles, entityManager, params[0]);
		return ska::NodeValue{ static_cast<long>(CenterPosition(entityManager, entity).x / world.getBlockSize()) };
	});

	m_builder.bindField("blockY", [&entityManager, &worldState, &handles](std::vector<ska::NodeValue> params) {
		const auto& world = worldState.getWorld();
		const auto entity = Entity(handles, entityManager, params[0]);
		return ska::NodeValue{ static_cast<long>(CenterPosition(entityManager, entity).y / world.getBlockSize()) };
	});

	m_builder.bindField("blockHasProperty", [&worldState](std::vector<ska::NodeValue> params) {
		const auto& world = worldState.getWorld();
		const auto layer = Int(params[0]);
		const auto x = Int(params[1]);
		const auto y = Int(params[2]);
		const auto property = static_cast<unsigned int>(Int(params[3]));
		if (layer < 0 || x < 0 || y < 0) {
			return ska::NodeValue{ false };
		}
		const auto* block = world.getCollisionProfile().getBlock(static_cast<std::size_t>(layer), static_cast<std::size_t>(x), static_cast<std::size_t>(y));
		return ska::NodeValue{ block != nullptr && (block->properties.bitMask & property) == property };
	});

	m_builder.bindField("blockFree", [&worldState](std::vector<ska::NodeValue> params) {
		const auto& world = worldState.getWorld();
		const auto x = Int(params[0]);
		const auto y = Int(params[1]);
		if (x < 0 || y < 0 || static_cast<std::size_t>(x) >= world.getBlocksX() || static_cast<std::size_t>(y) >= world.getBlocksY()) {
			return ska::NodeValue{ false };
		}
		// The physics : the walls (filled), the characters ; the outline of the water on the edges of the block is not inside
		const auto blockSize = static_cast<double>(world.getBlockSize());
		const auto margin = 4.;
		const auto box = cpBBNew(x * blockSize + margin, y * blockSize + margin, (x + 1) * blockSize - margin, (y + 1) * blockSize - margin);
		return ska::NodeValue{ !worldState.getSpace().shapeIn(box) };
	});

	m_builder.bindField("shake", [&worldState](std::vector<ska::NodeValue> params) {
		worldState.getCamera().shake(static_cast<unsigned int>(std::max(0, Int(params[0]))), Int(params[1]));
		return ska::NodeValue{};
	});

	m_builder.bindField("crumbling", [](std::vector<ska::NodeValue>) { NotAvailable("crumbling"); return ska::NodeValue{}; });

	m_builder.generate();
}

PartyScriptModule::PartyScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState) :
	GameScriptModule(vm, "party") {
	(void) entityManager;

	m_builder.bindField("addPokemon", [&ged, &worldState](std::vector<ska::NodeValue> params) {
		const auto id = Int(params[0]);
		const auto dataFile = "./Data/Monsters/" + std::to_string(id) + ".ini";
		if (!std::ifstream{ dataFile }.good()) {
			SLOG_STATIC(ska::LogLevel::Warn, GameScriptModule) << "Script : no data for the pokemon " << id << " (" << dataFile << ")";
			return ska::NodeValue{ false };
		}
		auto details = ska::IniReader{ dataFile };
		// Level 1 (no experience) : its hit points at this level (battle rules)
		const auto hp = static_cast<unsigned int>(worldState.getBattleRules().maxHp(details.get<int>("BaseStats hp"), 1));

		// Saved in the team of the savegame, then shown in the team
		auto team = Team{ worldState.getSaveGame().getPathName() };
		auto member = TeamMember{};
		member.id = id;
		member.hp = static_cast<int>(hp);
		if (!team.add(member).has_value()) {
			SLOG_STATIC(ska::LogLevel::Warn, GameScriptModule) << "Script : the team is full, the pokemon " << id << " is not added";
			return ska::NodeValue{ false };
		}
		auto loadEvent = EntityLoadEvent{ details, static_cast<unsigned int>(id), hp };
		ged.ska::Observable<EntityLoadEvent>::notifyObservers(loadEvent);
		return ska::NodeValue{ true };
	});

	m_builder.bindField("size", [&worldState](std::vector<ska::NodeValue>) {
		return ska::NodeValue{ static_cast<long>(Team{ worldState.getSaveGame().getPathName() }.size()) };
	});

	m_builder.bindField("hp", [&worldState](std::vector<ska::NodeValue> params) {
		const auto pokemon = Team{ worldState.getSaveGame().getPathName() }.member(Int(params[0]));
		return ska::NodeValue{ static_cast<long>(pokemon.has_value() ? pokemon->hp : 0) };
	});

	m_builder.bindField("level", [&worldState](std::vector<ska::NodeValue> params) {
		const auto pokemon = Team{ worldState.getSaveGame().getPathName() }.member(Int(params[0]));
		if (!pokemon.has_value()) {
			return ska::NodeValue{ 0L };
		}
		const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(pokemon->id) + ".ini" };
		const auto needed = data.exists("Experience level_1_exp_needed") ? data.get<int>("Experience level_1_exp_needed") : 0;
		return ska::NodeValue{ static_cast<long>(worldState.getBattleRules().level(pokemon->experience, needed)) };
	});

	m_builder.bindField("heal", [&worldState](std::vector<ska::NodeValue> params) {
		HealTeamMember(worldState.getSaveGame().getPathName(), Int(params[0]), Int(params[1]), worldState.getBattleRules());
		return ska::NodeValue{};
	});

	m_builder.bindField("pokemonOut", [](std::vector<ska::NodeValue>) { NotAvailable("pokemonOut"); return ska::NodeValue{}; });

	m_builder.generate();
}

InterfaceScriptModule::InterfaceScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState) :
	GameScriptModule(vm, "interface") {
	(void) entityManager;
	(void) ged;
	(void) worldState;

	m_builder.bindField("hideInterface", [&ged](std::vector<ska::NodeValue> params) {
		auto event = InterfaceEvent{};
		event.hidden = params[0].nodeval<bool>();
		ged.ska::Observable<InterfaceEvent>::notifyObservers(event);
		return ska::NodeValue{};
	});

	m_builder.bindField("shop", [](std::vector<ska::NodeValue>) { NotAvailable("shop"); return ska::NodeValue{}; });

	

	m_builder.generate();
}

BattleScriptModule::BattleScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState) :
	GameScriptModule(vm, "battle") {

	// A battle against wild Pokemon (the first one, and the others of its group), the script waits for its end
	auto startBattle = [&vm, &entityManager, &ged, &worldState](const std::vector<std::pair<int, unsigned int>>& monsters, bool canFlee) {
		if (monsters.empty()) {
			throw std::runtime_error("a battle needs at least one wild pokemon");
		}
		for (const auto& [monster, level] : monsters) {
			if (!std::ifstream{ "./Data/Monsters/" + std::to_string(monster) + ".ini" }.good()) {
				throw std::runtime_error("no data for the pokemon " + std::to_string(monster) + " (Data/Monsters)");
			}
		}
		const auto* trainer = worldState.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER);
		if (trainer == nullptr || !entityManager.hasComponent<ska::PositionComponent>(*trainer)) {
			throw std::runtime_error("a battle needs the player on the map");
		}

		// The wild Pokemon appears near the player (3 blocks on its right : the Pokemon of the player appears between them)
		const auto blockSize = static_cast<int>(worldState.getWorld().getBlockSize());
		const auto& position = entityManager.getComponent<ska::PositionComponent>(*trainer);
		const auto monster = monsters.front().first;
		const auto opponent = CustomEntityManager::createMovingCharacterNG(entityManager, worldState.getSpace(), ska::Point<int>( static_cast<int>(position.x) / blockSize + 3, static_cast<int>(position.y) / blockSize ), monster, blockSize);
		auto fight = FightComponent{};
		fight.level = monsters.front().second < 1 ? 1 : monsters.front().second;
		fight.opponentScriptId = monster;
		fight.companions.assign(monsters.begin() + 1, monsters.end());
		fight.canFlee = canFlee;
		entityManager.addComponent<FightComponent>(opponent, std::move(fight));

		// The script goes on once the battle is over (the battle counter changed), with its result
		const auto battles = worldState.battles();
		if (!StartBattle(entityManager, ged, worldState, *trainer, opponent)) {
			// No Pokemon of the team can fight : no battle, lost
			entityManager.removeEntity(opponent);
			return ska::NodeValue{ false };
		}
		vm.suspendUntil([&worldState, battles]() { return worldState.battles() != battles; },
			[&worldState]() { return ska::NodeValue{ worldState.lastBattleWon() }; });
		return ska::NodeValue{};
	};

	m_builder.bindField("start", [startBattle](std::vector<ska::NodeValue> params) {
		const auto level = Int(params[1]);
		return startBattle({ { Int(params[0]), static_cast<unsigned int>(level < 1 ? 1 : level) } }, true);
	});

	m_builder.bindField("startWith", [startBattle](std::vector<ska::NodeValue> params) {
		const auto& monsters = params[0].nodeval<ska::NodeValueArray>();
		const auto& levels = params[1].nodeval<ska::NodeValueArray>();
		if (monsters == nullptr || levels == nullptr || monsters->size() != levels->size()) {
			throw std::runtime_error("Battle.startWith : one level for each pokemon");
		}
		auto group = std::vector<std::pair<int, unsigned int>>{};
		for (std::size_t index = 0; index < monsters->size(); index++) {
			const auto level = (*levels)[index].nodeval<long>();
			group.emplace_back(static_cast<int>((*monsters)[index].nodeval<long>()), static_cast<unsigned int>(level < 1 ? 1 : level));
		}
		return startBattle(group, params[2].nodeval<bool>());
	});

	m_builder.bindField("battles", [&worldState](std::vector<ska::NodeValue>) {
		return ska::NodeValue{ static_cast<long>(worldState.battles()) };
	});

	m_builder.bindField("lastWon", [&worldState](std::vector<ska::NodeValue>) {
		return ska::NodeValue{ worldState.lastBattleWon() };
	});

	m_builder.bindField("lastFled", [&worldState](std::vector<ska::NodeValue>) {
		return ska::NodeValue{ worldState.lastBattleFled() };
	});

	m_builder.generate();
}

SaveScriptModule::SaveScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState) :
	GameScriptModule(vm, "save") {
	(void) ged;

	m_builder.bindField("save", [&entityManager, &worldState](std::vector<ska::NodeValue>) {
		const auto* trainer = worldState.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER);
		if (trainer == nullptr || !entityManager.hasComponent<ska::PositionComponent>(*trainer)) {
			return ska::NodeValue{ false };
		}
		// The block of the player, as it is placed when the game is loaded (its position)
		const auto blockSize = static_cast<int>(worldState.getWorld().getBlockSize());
		const auto& position = entityManager.getComponent<ska::PositionComponent>(*trainer);
		const auto blockX = static_cast<int>(position.x + blockSize / 2) / blockSize;
		const auto blockY = static_cast<int>(position.y + blockSize / 2) / blockSize;
		worldState.getSaveGame().saveGame(worldState.getMapName(), blockX, blockY);
		SLOG_STATIC(ska::LogLevel::Info, GameScriptModule) << "Game saved (" << worldState.getMapName() << ", " << blockX << ", " << blockY << ")";
		return ska::NodeValue{ true };
	});

	m_builder.generate();
}

InventoryScriptModule::InventoryScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState) :
	GameScriptModule(vm, "inventory") {
	(void) entityManager;
	(void) ged;
	auto& bag = worldState.getBag();

	m_builder.bindField("count", [&bag](std::vector<ska::NodeValue> params) {
		return ska::NodeValue{ static_cast<long>(bag.count(Int(params[0]))) };
	});
	m_builder.bindField("items", [&bag](std::vector<ska::NodeValue>) {
		auto items = std::make_shared<ska::NodeValueArrayRaw>();
		for (const auto item : bag.items()) {
			items->push_back(ska::NodeValue{ static_cast<long>(item) });
		}
		return ska::NodeValue{ ska::NodeValueArray{ std::move(items) } };
	});
	m_builder.bindField("add", [&bag](std::vector<ska::NodeValue> params) {
		bag.add(Int(params[0]), Int(params[1]));
		return ska::NodeValue{};
	});
	m_builder.bindField("remove", [&bag](std::vector<ska::NodeValue> params) {
		return ska::NodeValue{ bag.remove(Int(params[0]), Int(params[1])) };
	});
	m_builder.bindField("money", [&bag](std::vector<ska::NodeValue>) {
		return ska::NodeValue{ static_cast<long>(bag.money()) };
	});
	m_builder.bindField("earn", [&bag](std::vector<ska::NodeValue> params) {
		bag.earn(Int(params[0]));
		return ska::NodeValue{};
	});
	m_builder.bindField("pay", [&bag](std::vector<ska::NodeValue> params) {
		return ska::NodeValue{ bag.pay(Int(params[0])) };
	});
	m_builder.bindField("name", [](std::vector<ska::NodeValue> params) {
		const auto item = Bag::data(Int(params[0]));
		return ska::NodeValue{ std::make_shared<std::string>(item.has_value() ? item->name : "") };
	});
	m_builder.bindField("price", [](std::vector<ska::NodeValue> params) {
		const auto item = Bag::data(Int(params[0]));
		return ska::NodeValue{ static_cast<long>(item.has_value() ? item->price : 0) };
	});
	m_builder.bindField("use", [&bag, &worldState](std::vector<ska::NodeValue> params) {
		return ska::NodeValue{ UseItem(worldState, bag, Int(params[0]), Int(params[1])) };
	});

	m_builder.generate();
}

GameScriptModules::GameScriptModules(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState) :
	dialog(vm, entityManager, ged, worldState),
	character(vm, entityManager, ged, worldState),
	map(vm, entityManager, ged, worldState),
	party(vm, entityManager, ged, worldState),
	interface(vm, entityManager, ged, worldState),
	battle(vm, entityManager, ged, worldState),
	save(vm, entityManager, ged, worldState),
	inventory(vm, entityManager, ged, worldState) {
}
