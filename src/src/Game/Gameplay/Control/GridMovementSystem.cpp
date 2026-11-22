#include "Game/__internalConfig/LoggerConfig.h"
#include "GridMovementSystem.h"
#include <cmath>
#include <algorithm>
#include "Core/Inputs/InputContextManager.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/ECS/Basics/Physic/CollidableComponent.h"
#include "Core/ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Core/Data/Events/CollisionEvent.h"
#include "World/TileWorld.h"
#include "World/CollisionProfile.h"
#include <chipmunk/chipmunk.h>
#include "Physic/Space.h"
#include "Physic/Body.h"
#include "Physic/Vect.h"
#include "../World/WorldState.h"
#include "../CustomEntityManager.h"

namespace {
	// 0 : down, 1 : right, 2 : up, 3 : left
	ska::Point<float> DirectionVector(int direction) {
		switch (direction) {
		case 0: return { 0.F, 1.F };
		case 1: return { 1.F, 0.F };
		case 2: return { 0.F, -1.F };
		default: return { -1.F, 0.F };
		}
	}
}

GridMovementSystem::GridMovementSystem(ska::EntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState) :
	System(entityManager),
	SubObserver<ska::InputKeyEvent>(std::bind(&GridMovementSystem::onKeyInputEvent, this, std::placeholders::_1), ged),
	m_entities(entityManager),
	m_ged(ged),
	m_worldState(worldState),
	m_occupants(entityManager) {
}

bool GridMovementSystem::onKeyInputEvent(ska::InputKeyEvent& event) {
	// Several input contexts send their keys (map, interface) : the keys held are the ones of any of them since the last frame
	m_receivedToggles |= event.icm.getToggles();
	m_received = true;
	return true;
}

int GridMovementSystem::heldDirection(int facing) const {
	const ska::InputToggle keys[] = { ska::MoveDown, ska::MoveRight, ska::MoveUp, ska::MoveLeft };
	if (facing >= 0 && facing < 4 && m_toggles[keys[facing]]) {
		return facing;
	}
	for (auto direction = 0; direction < 4; direction++) {
		if (m_toggles[keys[direction]]) {
			return direction;
		}
	}
	return -1;
}

std::optional<ska::Point<float>> GridMovementSystem::nextBlock(ska::EntityId entity, int direction) const {
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	const auto& position = m_entities.getComponent<ska::PositionComponent>(entity);
	const auto vector = DirectionVector(direction);
	if (!m_entities.hasComponent<ska::cp::HitboxComponent>(entity)) {
		return ska::Point<float>{ std::round(position.x / blockSize) * blockSize + vector.x * blockSize, std::round(position.y / blockSize) * blockSize + vector.y * blockSize };
	}

	// On a block, the physics shape of the character is at the center of the block (it does not touch the walls on the
	// edges of the block, ex : the water, that starts surfing when touched)
	auto& space = m_worldState.getSpace();
	// The shape box comes from the position (the physics shape follows it one physics step late while walking) :
	// the shape is built at the center of the block whose corner is the position (CustomEntityManager)
	const auto physicsShape = space.shapeBox(m_entities.getComponent<ska::cp::HitboxComponent>(entity).shapeIndex);
	const auto shapeSize = ska::Point<float>{ static_cast<float>(std::round(physicsShape.r - physicsShape.l)), static_cast<float>(std::round(physicsShape.t - physicsShape.b)) };
	const auto shapeLeft = static_cast<float>(position.x) + std::floor((blockSize - shapeSize.x) / 2.F);
	const auto shapeBottom = static_cast<float>(position.y) + std::floor((blockSize - shapeSize.y) / 2.F);
	const auto shape = cpBBNew(shapeLeft, shapeBottom, shapeLeft + shapeSize.x, shapeBottom + shapeSize.y);
	// From the block it is on, to the next one
	const auto block = ska::Point<float>{ std::round(static_cast<float>(position.x) / blockSize), std::round(static_cast<float>(position.y) / blockSize) };
	const auto target = ska::Point<float>{ (block.x + vector.x) * blockSize, (block.y + vector.y) * blockSize };
	const auto shift = ska::Point<float>{ target.x - static_cast<float>(position.x), target.y - static_cast<float>(position.y) };
	const auto margin = 4.;
	const auto targetBox = cpBBNew(shape.l + shift.x + margin, shape.b + shift.y + margin, shape.r + shift.x - margin, shape.t + shift.y - margin);

	// The walls of the map (the same as the physics) : on the way from the block to the next one (some walls are thin lines
	// on the edges of the blocks), the sides excepted (the walls along the way)
	const auto sweptBox = cpBBNew(std::min(shape.l, shape.l + shift.x) + margin, std::min(shape.b, shape.b + shift.y) + margin,
		std::max(shape.r, shape.r + shift.x) - margin, std::max(shape.t, shape.t + shift.y) - margin);
	if (space.staticShapeIn(sweptBox)) {
		m_bumped = true;
		return {};
	}

	// No other character there, nor going there
	for (const auto other : m_occupants.getEntities()) {
		if (other == entity) {
			continue;
		}
		const auto otherShape = space.shapeBox(m_entities.getComponent<ska::cp::HitboxComponent>(other).shapeIndex);
		if (cpBBIntersects(targetBox, otherShape)) {
			return {};
		}
		if (m_entities.hasComponent<GridMovementComponent>(other)) {
			const auto& otherGrid = m_entities.getComponent<GridMovementComponent>(other);
			const auto& otherPosition = m_entities.getComponent<ska::PositionComponent>(other);
			if (otherGrid.stepping) {
				const auto otherShift = ska::Point<float>{ otherGrid.to.x - static_cast<float>(otherPosition.x), otherGrid.to.y - static_cast<float>(otherPosition.y) };
				if (cpBBIntersects(targetBox, cpBBNew(otherShape.l + otherShift.x, otherShape.b + otherShift.y, otherShape.r + otherShift.x, otherShape.t + otherShift.y))) {
					return {};
				}
			}
		}
	}
	return target;
}

void GridMovementSystem::place(ska::EntityId entity, ska::Point<float> position, ska::Point<float> velocity) {
	CustomEntityManager::place(m_entities, m_worldState.getSpace(), entity, position);
	// The physics body moves at the speed of the step (the walk animation follows the movement)
	auto& force = m_entities.getComponent<ska::ForceComponent>(entity);
	force.x = velocity.x / 10.F;
	force.y = velocity.y / 10.F;
	if (velocity.x == 0.F && velocity.y == 0.F && m_entities.hasComponent<ska::cp::HitboxComponent>(entity)) {
		const auto& body = m_entities.getComponent<ska::cp::HitboxComponent>(entity);
		auto& space = m_worldState.getSpace();
		space.getBody(body.bodyIndex).setVelocity(ska::cp::Vect{ 0., 0. });
		space.getBody(body.controlBodyIndex).setVelocity(ska::cp::Vect{ 0., 0. });
	}
}

void GridMovementSystem::refresh(unsigned int ellapsedTime) {
	if (m_received) {
		m_toggles = m_receivedToggles;
		m_receivedToggles.reset();
		m_received = false;
	}
	for (const auto entity : getEntities()) {
		auto& grid = m_componentAccessor.get<GridMovementComponent>(entity);
		auto* input = m_componentPossibleAccessor.get<ska::InputComponent>(entity);
		const auto keys = grid.mode == GridMovementComponent::Mode::Keys;

		// The power of the normal walk is kept, the free movement of the keys is disabled
		auto* mode = m_componentPossibleAccessor.get<MovementModeComponent>(entity);
		if (keys && input != nullptr) {
			if (mode == nullptr) {
				auto saved = MovementModeComponent{};
				saved.walkPower = input->movePower;
				saved.jumpPower = input->jumpPower;
				m_entities.addComponent<MovementModeComponent>(entity, std::move(saved));
			} else if (input->movePower > 0.F && mode->walkPower == 0.F) {
				mode->walkPower = input->movePower;
			}
			input->movePower = 0.F;
		}

		// Moved by a script : animated by its physics velocity
		auto& animation = m_componentAccessor.get<ska::AnimationComponent>(entity);
		if (m_componentPossibleAccessor.get<ska::IADefinedMovementComponent>(entity) != nullptr) {
			grid.stepping = false;
			animation.drivenUntil = 0;
			continue;
		}
		// Animated by its steps : a foot forward each half step, in the direction of the step (the physics velocity is
		// not exact : a walk to the right could be drawn as a diagonal)
		animation.drivenUntil = ska::TimeUtils::getTicks() + 200;
		animation.walking = grid.stepping;
		animation.walkDelay = std::max(60u, grid.duration / 2);
		animation.state = grid.stepping ? grid.stepDirection : grid.facing;

		const auto locked = mode != nullptr && mode->mode == "locked";
		const auto running = (mode != nullptr && mode->mode == "run") || m_toggles[ska::Run];
		const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());

		auto finished = false;
		// Moved away from its step by something else (ex : a script placing it while the keys were disabled) : no step
		if (grid.stepping) {
			const auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
			const auto distanceFrom = std::abs(static_cast<float>(position.x) - grid.from.x) + std::abs(static_cast<float>(position.y) - grid.from.y);
			const auto distanceTo = std::abs(static_cast<float>(position.x) - grid.to.x) + std::abs(static_cast<float>(position.y) - grid.to.y);
			if (distanceFrom > blockSize * 1.5F && distanceTo > blockSize * 1.5F) {
				grid.stepping = false;
			}
		}

		if (grid.stepping) {
			grid.elapsed += ellapsedTime;
			const auto progress = std::min(1.F, static_cast<float>(grid.elapsed) / static_cast<float>(grid.duration));
			const auto speed = blockSize * 1000.F / static_cast<float>(grid.duration);
			const auto vector = DirectionVector(grid.stepDirection);
			if (progress < 1.F) {
				const auto position = ska::Point<float>{ grid.from.x + (grid.to.x - grid.from.x) * progress, grid.from.y + (grid.to.y - grid.from.y) * progress };
				place(entity, position, { vector.x * speed, vector.y * speed });
				continue;
			}
			// On the block : the next step starts right away when the key is still held (no stop between the blocks)
			grid.stepping = false;
			finished = true;
			place(entity, grid.to, { vector.x * speed, vector.y * speed });
		}

		// Not on a block (moved by a script, by the physics, placed after a battle) : back on the nearest block
		if (!grid.stepping && !finished) {
			const auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
			const auto onBlock = ska::Point<float>{ std::round(static_cast<float>(position.x) / blockSize) * blockSize, std::round(static_cast<float>(position.y) / blockSize) * blockSize };
			if (onBlock.x != static_cast<float>(position.x) || onBlock.y != static_cast<float>(position.y)) {
				place(entity, onBlock, {});
			}
		}

		if (!keys) {
			startAutomaticStep(entity, grid, ellapsedTime, finished);
		} else if (input != nullptr && !m_worldState.getFade().running()) {
			// Without InputComponent (disabled, ex : during a cutscene), the player does not move ; nor during the fade of a
			// map change
			startStep(entity, grid, locked, running, ellapsedTime, finished);
		}
		if (finished && !grid.stepping) {
			place(entity, grid.to, {});
		}
	}
}

void GridMovementSystem::startStep(ska::EntityId entity, GridMovementComponent& grid, bool locked, bool running, unsigned int ellapsedTime, bool walking) {
	const auto direction = locked ? -1 : heldDirection(grid.facing);
	if (direction < 0) {
		grid.turning = false;
		grid.heldTime = 0;
		grid.bumpDirection = -1;
		return;
	}

	auto& animation = m_componentAccessor.get<ska::AnimationComponent>(entity);
	if (direction != grid.facing) {
		grid.facing = direction;
		animation.state = direction;
		grid.heldTime = 0;
		// From a stop, a short press only turns ; while walking, it turns and walks
		if (!walking) {
			grid.turning = true;
			return;
		}
	}
	grid.heldTime += ellapsedTime;
	if (grid.turning && grid.heldTime < TurnDelay) {
		return;
	}
	grid.turning = false;
	animation.state = direction;

	m_bumped = false;
	if (step(entity, grid, direction, running ? RunStep : WalkStep)) {
		grid.bumpDirection = -1;
	} else if (m_bumped && grid.bumpDirection != direction) {
		// Against a wall : it is touched once (its scripts, ex : the water, to surf), the character stays on its block
		grid.bumpDirection = direction;
		touch(entity, direction);
	}
}

void GridMovementSystem::touch(ska::EntityId entity, int direction) {
	const auto blockSize = static_cast<int>(m_worldState.getWorld().getBlockSize());
	const auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
	const auto vector = DirectionVector(direction);
	const auto block = ska::Point<int>{ static_cast<int>(std::round(static_cast<float>(position.x) / static_cast<float>(blockSize))), static_cast<int>(std::round(static_cast<float>(position.y) / static_cast<float>(blockSize))) };
	const auto wall = ska::Point<int>{ block.x + static_cast<int>(vector.x), block.y + static_cast<int>(vector.y) };
	auto wcc = ska::WorldCollisionComponent{};
	// The wall, and the block of the character (as the physics contacts : ex the water touched from inside, to leave it)
	wcc.blockContacts.push_back(wall * blockSize);
	wcc.blockContacts.push_back(block * blockSize);
	// From the wall to the block of the character
	wcc.lastBlockDirection = ska::Point<int>{ -static_cast<int>(vector.x), -static_cast<int>(vector.y) };
	wcc.normal = ska::Point<float>{ -vector.x, -vector.y };
	const auto collidable = ska::CollidableComponent{};
	auto event = ska::CollisionEvent{ entity, &wcc, nullptr, collidable };
	m_ged.ska::Observable<ska::CollisionEvent>::notifyObservers(event);
}

bool GridMovementSystem::step(ska::EntityId entity, GridMovementComponent& grid, int direction, unsigned int duration) {
	const auto target = nextBlock(entity, direction);
	if (!target.has_value()) {
		return false;
	}
	const auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
	grid.stepping = true;
	grid.stepDirection = direction;
	grid.from = { static_cast<float>(position.x), static_cast<float>(position.y) };
	grid.to = *target;
	grid.elapsed = 0;
	grid.duration = duration;
	return true;
}

void GridMovementSystem::startAutomaticStep(ska::EntityId entity, GridMovementComponent& grid, unsigned int ellapsedTime, bool walking) {
	// A wandering character waits between its steps, a path is walked without stopping (it waits at the end of a loop)
	if (grid.wait > ellapsedTime) {
		grid.wait -= ellapsedTime;
		return;
	}
	grid.wait = 0;
	auto& animation = m_componentAccessor.get<ska::AnimationComponent>(entity);
	if (grid.mode == GridMovementComponent::Mode::Path) {
		if (grid.path.empty()) {
			return;
		}
		const auto direction = grid.path[grid.pathIndex % grid.path.size()];
		grid.facing = direction;
		animation.state = direction;
		if (step(entity, grid, direction, WalkStep)) {
			grid.pathIndex = (grid.pathIndex + 1) % grid.path.size();
			if (grid.pathIndex == 0) {
				grid.wait = 1000;
			}
		} else {
			// The way is not free : it waits and tries again
			grid.wait = 500;
		}
		return;
	}

	// Wander : turns to a random direction, sometimes walks
	const auto direction = static_cast<int>(std::uniform_int_distribution<int>{ 0, 3 }(m_random));
	grid.facing = direction;
	animation.state = direction;
	if (std::uniform_int_distribution<int>{ 0, 2 }(m_random) > 0) {
		step(entity, grid, direction, WalkStep);
	}
	grid.wait = static_cast<unsigned int>(std::uniform_int_distribution<int>{ 1000, 3000 }(m_random));
}
