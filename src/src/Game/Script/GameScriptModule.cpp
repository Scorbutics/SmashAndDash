#include "Game/__internalConfig/LoggerConfig.h"
#include "GameScriptModule.h"
#include <limits>
#include "Scripting/ScriptVM.h"
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
#include "World/TileWorld.h"
#include "World/CollisionProfile.h"
#include "Physic/Space.h"
#include "../Gameplay/CustomEntityManager.h"
#include "../Gameplay/Data/MapEvent.h"
#include "../Gameplay/Data/DialogEvent.h"

namespace {
	ska::EntityId Entity(ska::EntityManager& entityManager, const ska::NodeValue& value) {
		const auto entity = value.nodeval<long>();
		if (entity < 0 || !entityManager.hasComponent<ska::PositionComponent>(static_cast<ska::EntityId>(entity))) {
			throw std::runtime_error("invalid entity : " + std::to_string(entity));
		}
		return static_cast<ska::EntityId>(entity);
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
}

GameScriptModule::GameScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, const ska::TileWorld& world, ska::cp::Space& space) :
	ska::lang::Module<ska::bytecode::Interpreter> { vm.modules(), "game", "std:game" },
	m_builder(ska::BridgeBuilder<ska::bytecode::Interpreter> { m_bridge }) {

	m_builder.bindField("move", [&vm, &entityManager](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(entityManager, params[0]);
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
		definedMovement.delay = std::numeric_limits<unsigned int>::max();
		const auto force = ska::PhysicUtils::getMovement(direction, distance);
		definedMovement.directions.push_back(ska::Point<int>::cartesian(static_cast<int>(force.power), force.angle));
		entityManager.addComponent<ska::IADefinedMovementComponent>(entity, std::move(definedMovement));

		// The script goes on once the movement is done (the movement component is removed)
		vm.suspendUntil([&entityManager, entity, wasGhost]() {
			if (entityManager.hasComponent<ska::IADefinedMovementComponent>(entity)) {
				return false;
			}
			if (entityManager.hasComponent<ska::CollidableComponent>(entity)) {
				entityManager.getComponent<ska::CollidableComponent>(entity).ghost = wasGhost;
			}
			return true;
		});
		return ska::NodeValue{};
	});

	m_builder.bindField("jump", [&entityManager](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(entityManager, params[0]);
		Require<ska::MovementComponent>(entityManager, entity, "jump").vz = static_cast<float>(Int(params[1]));
		return ska::NodeValue{};
	});

	m_builder.bindField("message", [&vm, &ged](std::vector<ska::NodeValue> params) {
		auto dialog = DialogEvent{ String(params[0]), "scriptDialog", ska::Rectangle{ 0, std::numeric_limits<int>::max(), -1, 4 * 48 }, true };
		auto closed = std::make_shared<bool>(false);
		dialog.onClose = [closed]() { *closed = true; };
		ged.ska::Observable<DialogEvent>::notifyObservers(dialog);
		vm.suspendUntil([closed]() { return *closed; });
		return ska::NodeValue{};
	});

	m_builder.bindField("teleport", [&ged, &world](std::vector<ska::NodeValue> params) {
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

	m_builder.bindField("addCharacter", [&entityManager, &space, &world](std::vector<ska::NodeValue> params) {
		const auto character = CustomEntityManager::createCharacterNG(entityManager, space, ska::Point<int>{ Int(params[1]), Int(params[2]) },
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
		return ska::NodeValue{ static_cast<long>(character) };
	});

	m_builder.bindField("playAnimation", [&entityManager](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(entityManager, params[0]);
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

	m_builder.bindField("direction", [&entityManager](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(entityManager, params[0]);
		return ska::NodeValue{ static_cast<long>(Require<ska::AnimationComponent>(entityManager, entity, "have a direction").state) };
	});

	m_builder.bindField("setDirection", [&entityManager](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(entityManager, params[0]);
		Require<ska::AnimationComponent>(entityManager, entity, "have a direction").state = params[1].nodeval<long>();
		return ska::NodeValue{};
	});

	m_builder.bindField("setLayer", [&entityManager](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(entityManager, params[0]);
		entityManager.getComponent<ska::PositionComponent>(entity).layer = static_cast<std::size_t>(std::max(0, Int(params[1])));
		return ska::NodeValue{};
	});

	m_builder.bindField("blockX", [&entityManager, &world](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(entityManager, params[0]);
		return ska::NodeValue{ static_cast<long>(CenterPosition(entityManager, entity).x / world.getBlockSize()) };
	});

	m_builder.bindField("blockY", [&entityManager, &world](std::vector<ska::NodeValue> params) {
		const auto entity = Entity(entityManager, params[0]);
		return ska::NodeValue{ static_cast<long>(CenterPosition(entityManager, entity).y / world.getBlockSize()) };
	});

	m_builder.bindField("blockHasProperty", [&world](std::vector<ska::NodeValue> params) {
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

	m_builder.generate();
}
