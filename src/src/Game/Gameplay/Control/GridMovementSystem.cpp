#include "Game/__internalConfig/LoggerConfig.h"
#include "GridMovementSystem.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include "Core/Inputs/InputContextManager.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/ECS/Basics/Physic/CollidableComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Core/Data/Events/CollisionEvent.h"
#include "Core/ECS/Basics/Script/ScriptAwareComponent.h"
#include "World/TileWorld.h"
#include "World/CollisionProfile.h"
#include <chipmunk/chipmunk.h>
#include "Physic/Space.h"
#include "Physic/Body.h"
#include "Physic/Vect.h"
#include "../World/WorldState.h"
#include "../CustomEntityManager.h"
#include "../../Graphic/CharsetSheet.h"

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
	if (event.icm.getActions()[ska::Jump]) {
		m_jumpPressed = true;
	}
	return true;
}

bool GridMovementSystem::wallAhead(ska::EntityId entity, int direction, float shift) const {
	if (!m_entities.hasComponent<ska::cp::HitboxComponent>(entity)) {
		return false;
	}
	const auto& space = m_worldState.getSpace();
	const auto shape = space.shapeBox(m_entities.getComponent<ska::cp::HitboxComponent>(entity).shapeIndex);
	// A strip along the side of the shape that faces the direction, "Probe" pixels deep, its ends cut off (a wall along
	// the shape, touched by its sides, is not in front of it)
	constexpr auto Probe = 2.;
	constexpr auto Cut = 2.;
	const auto vector = DirectionVector(direction);
	const auto across = ska::Point<double>{ vector.y != 0.F ? shift : 0., vector.x != 0.F ? shift : 0. };
	auto strip = cpBB{};
	switch (direction) {
	case 0: strip = cpBBNew(shape.l + Cut, shape.t, shape.r - Cut, shape.t + Probe); break;
	case 1: strip = cpBBNew(shape.r, shape.b + Cut, shape.r + Probe, shape.t - Cut); break;
	case 2: strip = cpBBNew(shape.l + Cut, shape.b - Probe, shape.r - Cut, shape.b); break;
	default: strip = cpBBNew(shape.l - Probe, shape.b + Cut, shape.l, shape.t - Cut); break;
	}
	return space.staticShapeIn(cpBBNew(strip.l + across.x, strip.b + across.y, strip.r + across.x, strip.t + across.y));
}

int GridMovementSystem::cornerSide(ska::EntityId entity, int direction) const {
	if (!wallAhead(entity, direction, 0.F)) {
		return 0;
	}
	const auto& space = m_worldState.getSpace();
	const auto shape = space.shapeBox(m_entities.getComponent<ska::cp::HitboxComponent>(entity).shapeIndex);
	const auto vector = DirectionVector(direction);
	// The nearest side first : the way in front is free there, and the way aside to it too
	for (auto shift = 1; shift <= CornerSlide; shift++) {
		for (const auto side : { -1, 1 }) {
			const auto offset = static_cast<double>(shift * side);
			const auto across = ska::Point<double>{ vector.y != 0.F ? offset : 0., vector.x != 0.F ? offset : 0. };
			constexpr auto Inset = 1.;
			const auto aside = cpBBNew(std::min(shape.l, shape.l + across.x) + Inset, std::min(shape.b, shape.b + across.y) + Inset,
				std::max(shape.r, shape.r + across.x) - Inset, std::max(shape.t, shape.t + across.y) - Inset);
			if (!wallAhead(entity, direction, static_cast<float>(offset)) && !space.staticShapeIn(aside)) {
				return side;
			}
		}
	}
	return 0;
}

std::optional<ska::Point<float>> GridMovementSystem::nextBlock(ska::EntityId entity, int direction) const {
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	const auto& position = m_entities.getComponent<ska::PositionComponent>(entity);
	const auto vector = DirectionVector(direction);
	if (!m_entities.hasComponent<ska::cp::HitboxComponent>(entity)) {
		return ska::Point<float>{ std::round(position.x / blockSize) * blockSize + vector.x * blockSize, std::round(position.y / blockSize) * blockSize + vector.y * blockSize };
	}

	// On a block, the physics shape of the character does not touch the walls on the edges of the block (ex : the water,
	// that starts surfing when touched)
	auto& space = m_worldState.getSpace();
	// The shape box comes from the position (the physics shape follows it one physics step late while walking) :
	// the shape is built in the block whose corner is the position (CustomEntityManager::physicsShapeOffset)
	const auto physicsShape = space.shapeBox(m_entities.getComponent<ska::cp::HitboxComponent>(entity).shapeIndex);
	const auto shapeSize = ska::Point<float>{ static_cast<float>(std::round(physicsShape.r - physicsShape.l)), static_cast<float>(std::round(physicsShape.t - physicsShape.b)) };
	const auto offset = CustomEntityManager::physicsShapeOffset(ska::Point<int>{ static_cast<int>(shapeSize.x), static_cast<int>(shapeSize.y) }, static_cast<int>(blockSize));
	const auto shapeLeft = static_cast<float>(position.x) + static_cast<float>(offset.x);
	const auto shapeBottom = static_cast<float>(position.y) + static_cast<float>(offset.y);
	const auto shape = cpBBNew(shapeLeft, shapeBottom, shapeLeft + shapeSize.x, shapeBottom + shapeSize.y);
	// From the block it is on, to the next one
	const auto block = ska::Point<float>{ std::round(static_cast<float>(position.x) / blockSize), std::round(static_cast<float>(position.y) / blockSize) };
	const auto target = ska::Point<float>{ (block.x + vector.x) * blockSize, (block.y + vector.y) * blockSize };
	const auto shift = ska::Point<float>{ target.x - static_cast<float>(position.x), target.y - static_cast<float>(position.y) };
	// (6 pixels : the shape, 32 pixels wide from 8 to 40 in its block, stays off the outer cells of the masks of the tiles,
	// 12 pixels each : a rim along the side of a block does not stop a walk along it ; touching counts for the physics)
	const auto margin = 6.;
	const auto targetBox = cpBBNew(shape.l + shift.x + margin, shape.b + shift.y + margin, shape.r + shift.x - margin, shape.t + shift.y - margin);

	// Out of the map : a wall (its edges lead to the neighbouring maps by teleports on the edge blocks themselves ; an edge
	// with no way to its neighbour, ex a cliff there, is a dead end)
	const auto& world = m_worldState.getWorld();
	const auto next = ska::Point<int>{ static_cast<int>(block.x + vector.x), static_cast<int>(block.y + vector.y) };
	if (next.x < 0 || next.y < 0 || static_cast<std::size_t>(next.x) >= world.getBlocksX() || static_cast<std::size_t>(next.y) >= world.getBlocksY()) {
		m_bumped = true;
		return {};
	}

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
			// It stands there : the step walks into it
			m_blockedBy = other;
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
	const auto jumpPressed = m_jumpPressed;
	m_jumpPressed = false;
	for (const auto entity : getEntities()) {
		auto& grid = m_componentAccessor.get<GridMovementComponent>(entity);
		auto* input = m_componentPossibleAccessor.get<ska::InputComponent>(entity);
		grid.touched.reset();
		if (grid.seeing) {
			grid.seen += ellapsedTime;
		}
		const auto keys = grid.mode == GridMovementComponent::Mode::Keys;
		// The player touches the walls it pushes against (walkFreely), not the ones it walks along
		if (keys && m_entities.hasComponent<ska::ScriptAwareComponent>(entity)) {
			m_entities.getComponent<ska::ScriptAwareComponent>(entity).physicsTouches = false;
		}

		// The power of the normal walk is kept, the free movement of the keys is disabled
		auto* mode = m_componentPossibleAccessor.get<MovementModeComponent>(entity);
		if (keys && input != nullptr) {
			if (mode == nullptr) {
				auto saved = MovementModeComponent{};
				saved.walkPower = input->movePower;
				saved.jumpPower = input->jumpPower;
				m_entities.addComponent<MovementModeComponent>(entity, std::move(saved));
			} else {
				if (input->movePower > 0.F && mode->walkPower == 0.F) {
					mode->walkPower = input->movePower;
				}
				if (input->jumpPower > 0.F && mode->jumpPower == 0.F) {
					mode->jumpPower = input->jumpPower;
				}
			}
			input->movePower = 0.F;
			// The jump key hops (below), not the physics jump
			input->jumpPower = 0.F;
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

		// The hop : the height follows its arc, then the character is back on the ground
		if (grid.hopDuration > 0) {
			// The player jumps (the jump key, a ledge), it lands
			if (keys && grid.hopElapsed == 0 && grid.hopHeight > 0.F) {
				m_sounds.play(MapSound::Jump, 0.7F);
			}
			grid.hopElapsed += ellapsedTime;
			const auto progress = std::min(1.F, static_cast<float>(grid.hopElapsed) / static_cast<float>(grid.hopDuration));
			auto& position = m_entities.getComponent<ska::PositionComponent>(entity);
			position.z = static_cast<long>(std::lround(4.F * grid.hopHeight * progress * (1.F - progress)));
			if (m_entities.hasComponent<ska::MovementComponent>(entity)) {
				m_entities.getComponent<ska::MovementComponent>(entity).vz = 0.F;
			}
			if (progress >= 1.F) {
				if (keys && grid.hopHeight > 0.F) {
					m_sounds.play(MapSound::Land, 0.6F);
				}
				position.z = 0;
				grid.hopDuration = 0;
				grid.hopElapsed = 0;
				grid.hopHeight = 0.F;
			}
		}

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

		// Not on a block (moved by a script, by the physics, placed after a battle) : back on the nearest block (the player
		// walks freely)
		if (!keys && !grid.stepping && !finished) {
			const auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
			const auto onBlock = ska::Point<float>{ std::round(static_cast<float>(position.x) / blockSize) * blockSize, std::round(static_cast<float>(position.y) / blockSize) * blockSize };
			if (onBlock.x != static_cast<float>(position.x) || onBlock.y != static_cast<float>(position.y)) {
				place(entity, onBlock, {});
			}
		}

		// Forced steps (a script, leaving a door) : one after another without stopping, once the screen shows the map
		if (grid.forcedSteps > 0 || grid.forcedGhost) {
			if (!grid.stepping && grid.forcedSteps > 0 && m_worldState.getFade().showing()) {
				forcedStep(entity, grid);
			} else if (!grid.stepping && grid.forcedSteps == 0) {
				// The last one is done : back to its physics
				if (m_entities.hasComponent<ska::CollidableComponent>(entity)) {
					m_entities.getComponent<ska::CollidableComponent>(entity).ghost = grid.savedGhost;
				}
				grid.forcedGhost = false;
			}
			if (finished && !grid.stepping) {
				place(entity, grid.to, {});
			}
			continue;
		}

		if (!keys) {
			startAutomaticStep(entity, grid, ellapsedTime, finished);
		} else if (input != nullptr && !m_worldState.getFade().running()) {
			// Without InputComponent (disabled, ex : during a cutscene), the player does not move ; nor during the fade of a
			// map change. The jump key : a hop, standing or walking
			if (jumpPressed && !locked && mode != nullptr && mode->jumpPower > 0.F && !grid.stepping && grid.hopDuration == 0) {
				grid.hopDuration = HopDuration;
				grid.hopElapsed = 0;
				grid.hopHeight = HopHeight;
			}
			walkFreely(entity, grid, !locked, running, ellapsedTime);
		} else {
			// It stops at once
			grid.velocity = {};
			walkFreely(entity, grid, false, false, ellapsedTime);
		}
		if (finished && !grid.stepping) {
			place(entity, grid.to, {});
		}
	}
}

void GridMovementSystem::walkFreely(ska::EntityId entity, GridMovementComponent& grid, bool controlled, bool running, unsigned int ellapsedTime) {
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	auto dx = 0;
	auto dy = 0;
	if (controlled) {
		dx = (m_toggles[ska::MoveRight] ? 1 : 0) - (m_toggles[ska::MoveLeft] ? 1 : 0);
		dy = (m_toggles[ska::MoveDown] ? 1 : 0) - (m_toggles[ska::MoveUp] ? 1 : 0);
	}
	const auto moving = dx != 0 || dy != 0;
	// One of the 4 directions : the one it faces while it is still held (a diagonal keeps it), else the one held
	if (moving) {
		const auto keeps = (grid.facing == 0 && dy > 0) || (grid.facing == 1 && dx > 0) || (grid.facing == 2 && dy < 0) || (grid.facing == 3 && dx < 0);
		if (!keeps) {
			grid.facing = dx != 0 ? (dx > 0 ? 1 : 3) : (dy > 0 ? 0 : 2);
		}
	}
	// Along one of the 4 directions only (a diagonal slides along the walls by itself)
	const auto straight = moving && (dx == 0 || dy == 0);
	const auto direction = !straight ? -1 : (dx > 0 ? 1 : dx < 0 ? 3 : dy > 0 ? 0 : 2);

	const auto speed = blockSize * 1000.F / static_cast<float>(running ? RunStep : WalkStep);
	auto target = ska::Point<float>{ static_cast<float>(dx), static_cast<float>(dy) };
	if (dx != 0 && dy != 0) {
		target.x *= 0.7071F;
		target.y *= 0.7071F;
	}
	target.x *= speed;
	target.y *= speed;
	// Around the corner of a wall it nearly passes
	auto side = 0;
	if (straight) {
		side = cornerSide(entity, direction);
		if (side != 0) {
			if (dx == 0) {
				target.x = static_cast<float>(side) * speed;
			} else {
				target.y = static_cast<float>(side) * speed;
			}
		}
	}

	// Its velocity goes towards the one of the keys in a few frames
	const auto dt = static_cast<float>(ellapsedTime);
	const auto rate = 1.F - std::exp(-dt / (moving ? Accel : Decel));
	grid.velocity.x += (target.x - grid.velocity.x) * rate;
	grid.velocity.y += (target.y - grid.velocity.y) * rate;
	if (!moving && std::abs(grid.velocity.x) + std::abs(grid.velocity.y) < 8.F) {
		grid.velocity = {};
	}

	// It stays on the map (its edges lead to the neighbouring maps by teleports on the edge blocks themselves)
	if (m_entities.hasComponent<ska::cp::HitboxComponent>(entity)) {
		const auto& world = m_worldState.getWorld();
		const auto shape = m_worldState.getSpace().shapeBox(m_entities.getComponent<ska::cp::HitboxComponent>(entity).shapeIndex);
		const auto width = static_cast<double>(world.getBlocksX()) * blockSize;
		const auto height = static_cast<double>(world.getBlocksY()) * blockSize;
		const auto seconds = static_cast<double>(dt) / 1000.;
		if ((grid.velocity.x < 0.F && shape.l + grid.velocity.x * seconds < 0.) || (grid.velocity.x > 0.F && shape.r + grid.velocity.x * seconds > width)) {
			grid.velocity.x = 0.F;
		}
		if ((grid.velocity.y < 0.F && shape.b + grid.velocity.y * seconds < 0.) || (grid.velocity.y > 0.F && shape.t + grid.velocity.y * seconds > height)) {
			grid.velocity.y = 0.F;
		}
	}

	// The physics body moves at that velocity
	auto& force = m_entities.getComponent<ska::ForceComponent>(entity);
	force.x = grid.velocity.x / 10.F;
	force.y = grid.velocity.y / 10.F;
	if (grid.velocity.x == 0.F && grid.velocity.y == 0.F && m_entities.hasComponent<ska::cp::HitboxComponent>(entity)) {
		const auto& body = m_entities.getComponent<ska::cp::HitboxComponent>(entity);
		auto& space = m_worldState.getSpace();
		space.getBody(body.bodyIndex).setVelocity(ska::cp::Vect{ 0., 0. });
		space.getBody(body.controlBodyIndex).setVelocity(ska::cp::Vect{ 0., 0. });
	}

	// Its gait changes : a pose between the two (the run and the poses : the sheets of 9 columns, Charset)
	const auto gait = !moving ? 0 : running ? 2 : 1;
	if (gait != grid.gait) {
		auto pose = -1;
		auto duration = 0u;
		if (grid.gait == 0) {
			pose = Charset::SetOffPose;
			duration = SetOffTime;
		} else if (gait == 2) {
			pose = Charset::PushOffPose;
			duration = PushOffTime;
		} else if (grid.gait == 2) {
			pose = Charset::BrakePose;
			duration = gait == 0 ? BrakeTime : BrakeTime * 2 / 3;
		}
		if (pose >= 0) {
			m_componentAccessor.get<ska::AnimationComponent>(entity).pose = pose;
			grid.poseUntil = ska::TimeUtils::getTicks() + duration;
		}
		grid.gait = gait;
	}

	// Animated by the keys : a foot forward each half block, in the direction held (8 directions : 4 down right, 5 up
	// right, 6 up left, 7 down left), facing one of the 4 when still
	auto& animation = m_componentAccessor.get<ska::AnimationComponent>(entity);
	const auto current = std::sqrt(grid.velocity.x * grid.velocity.x + grid.velocity.y * grid.velocity.y);
	animation.drivenUntil = ska::TimeUtils::getTicks() + 200;
	animation.walking = moving;
	animation.walkDelay = std::max(60u, static_cast<unsigned int>(blockSize * 1000.F / std::max(current, speed * 0.6F) / 2.F));
	if (!moving) {
		animation.state = grid.facing;
	} else if (dx != 0 && dy != 0) {
		animation.state = dx > 0 ? (dy > 0 ? 4 : 5) : (dy < 0 ? 6 : 7);
	} else {
		animation.state = grid.facing;
	}

	// Pushing against a wall : it is touched once (its scripts, ex : the water, to surf)
	const auto againstWall = straight && wallAhead(entity, direction, 0.F);
	if (againstWall) {
		if (grid.bumpDirection != direction) {
			grid.bumpDirection = direction;
			touch(entity, direction);
		}
	} else {
		grid.bumpDirection = -1;
	}
	if (controlled) {
		// Stuck : against the wall, not sliding around its corner, on the ground
		walkSounds(entity, gait == 2, againstWall && side == 0 && grid.hopDuration == 0, ellapsedTime);
	} else {
		m_runDistance = -1.F;
		m_stuckTime = -1;
	}

	// The character its shape touches (ex : a wild Pokemon, a battle starts)
	if (m_entities.hasComponent<ska::cp::HitboxComponent>(entity)) {
		const auto& space = m_worldState.getSpace();
		const auto shape = space.shapeBox(m_entities.getComponent<ska::cp::HitboxComponent>(entity).shapeIndex);
		constexpr auto Reach = 2.;
		const auto around = cpBBNew(shape.l - Reach, shape.b - Reach, shape.r + Reach, shape.t + Reach);
		for (const auto other : m_occupants.getEntities()) {
			if (other != entity && cpBBIntersects(around, space.shapeBox(m_entities.getComponent<ska::cp::HitboxComponent>(other).shapeIndex))) {
				grid.touched = other;
				break;
			}
		}
	}
}

void GridMovementSystem::walkSounds(ska::EntityId entity, bool running, bool stuck, unsigned int ellapsedTime) {
	// The bump of the wall, again and again while it pushes
	if (!stuck) {
		m_stuckTime = -1;
	} else if (m_stuckTime < 0) {
		m_stuckTime = static_cast<int>(BumpRepeat - BumpDelay);
	} else {
		m_stuckTime += static_cast<int>(ellapsedTime);
		if (m_stuckTime >= static_cast<int>(BumpRepeat)) {
			m_stuckTime -= static_cast<int>(BumpRepeat);
			m_sounds.play(MapSound::Bump, 0.8F);
		}
	}

	// Its steps when it runs on the ground (not surfing : layer 0, Levels/surf.miniska ; not in the air) : a soft one each
	// block, the first one soon after it sets off
	const auto& grid = m_componentAccessor.get<GridMovementComponent>(entity);
	const auto onGround = m_componentAccessor.get<ska::PositionComponent>(entity).layer > 0 && grid.hopDuration == 0;
	if (!running || stuck || !onGround) {
		m_runDistance = -1.F;
		return;
	}
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	const auto speed = std::sqrt(grid.velocity.x * grid.velocity.x + grid.velocity.y * grid.velocity.y);
	if (m_runDistance < 0.F) {
		m_runDistance = blockSize * 0.6F;
	}
	m_runDistance += speed * static_cast<float>(ellapsedTime) / 1000.F;
	if (m_runDistance >= blockSize) {
		m_runDistance -= blockSize;
		m_runFoot = !m_runFoot;
		m_sounds.play(m_runFoot ? MapSound::RunStep : MapSound::RunStepOther, m_runFoot ? RunStepGain : RunStepGain * 0.8F);
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

void GridMovementSystem::forcedStep(ska::EntityId entity, GridMovementComponent& grid) {
	if (!grid.forcedGhost && m_entities.hasComponent<ska::CollidableComponent>(entity)) {
		auto& collidable = m_entities.getComponent<ska::CollidableComponent>(entity);
		grid.savedGhost = collidable.ghost;
		collidable.ghost = true;
		grid.forcedGhost = true;
	}
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	const auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
	const auto vector = DirectionVector(grid.forcedDirection);
	grid.forcedSteps--;
	grid.stepping = true;
	grid.facing = grid.forcedDirection;
	grid.stepDirection = grid.forcedDirection;
	grid.from = { static_cast<float>(position.x), static_cast<float>(position.y) };
	grid.to = { (std::round(grid.from.x / blockSize) + vector.x) * blockSize, (std::round(grid.from.y / blockSize) + vector.y) * blockSize };
	grid.elapsed = 0;
	grid.duration = grid.forcedDuration;
	m_componentAccessor.get<ska::AnimationComponent>(entity).state = grid.forcedDirection;
}

bool GridMovementSystem::step(ska::EntityId entity, GridMovementComponent& grid, int direction, unsigned int duration) {
	m_blockedBy.reset();
	const auto target = nextBlock(entity, direction);
	if (!target.has_value()) {
		if (m_blockedBy.has_value()) {
			grid.touched = m_blockedBy;
		}
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

bool GridMovementSystem::react(ska::EntityId entity, GridMovementComponent& grid, unsigned int ellapsedTime) {
	if (grid.mode != GridMovementComponent::Mode::Wander || grid.reaction == GridMovementComponent::Reaction::None || !grid.reactTo.has_value() ||
		!m_entities.hasComponent<ska::PositionComponent>(*grid.reactTo)) {
		grid.seeing = false;
		return false;
	}
	// In blocks : where it is, where the other one is
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	const auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
	const auto& other = m_entities.getComponent<ska::PositionComponent>(*grid.reactTo);
	const auto dx = static_cast<int>(std::lround(static_cast<float>(other.x) / blockSize)) - static_cast<int>(std::lround(static_cast<float>(position.x) / blockSize));
	const auto dy = static_cast<int>(std::lround(static_cast<float>(other.y) / blockSize)) - static_cast<int>(std::lround(static_cast<float>(position.y) / blockSize));
	if (std::abs(dx) + std::abs(dy) > grid.sight) {
		grid.seeing = false;
		return false;
	}
	auto& animation = m_componentAccessor.get<ska::AnimationComponent>(entity);
	// 0 : down, 1 : right, 2 : up, 3 : left
	const auto towardsX = dx > 0 ? 1 : 3;
	const auto towardsY = dy > 0 ? 0 : 2;
	const auto towards = std::abs(dx) >= std::abs(dy) ? (dx != 0 ? towardsX : towardsY) : towardsY;
	if (!grid.seeing) {
		// It notices it : it turns to it and stops a moment (its mark shows)
		grid.seeing = true;
		grid.seen = 0;
		grid.facing = towards;
		animation.state = towards;
		grid.wait = NoticeDelay;
		SLOG(ska::LogLevel::Info) << "Grid : a character at block (" << std::lround(static_cast<float>(position.x) / blockSize) << ", " << std::lround(static_cast<float>(position.y) / blockSize)
			<< ") notices the one it " << (grid.reaction == GridMovementComponent::Reaction::Approach ? "approaches" : "avoids");
		return true;
	}
	if (grid.wait > ellapsedTime) {
		grid.wait -= ellapsedTime;
		return true;
	}
	grid.wait = 0;

	// The directions to try, the best first
	auto directions = std::vector<int>{};
	if (grid.reaction == GridMovementComponent::Reaction::Approach) {
		// Along the longest way first ; next to it, it walks into it (the step is refused : it touches it)
		const auto first = std::abs(dx) >= std::abs(dy) ? (dx != 0 ? towardsX : towardsY) : towardsY;
		directions.push_back(first);
		if (dx != 0 && dy != 0) {
			directions.push_back(first == towardsX ? towardsY : towardsX);
		}
	} else {
		// Away : straight away first, then aside, never towards it
		const auto awayX = dx > 0 ? 3 : 1;
		const auto awayY = dy > 0 ? 2 : 0;
		const auto first = std::abs(dx) >= std::abs(dy) ? (dx != 0 ? awayX : awayY) : awayY;
		directions.push_back(first);
		directions.push_back(first == awayX || first == towardsX ? awayY : awayX);
		const auto side = std::uniform_int_distribution<int>{ 0, 1 }(m_random) == 0;
		if (dx == 0) {
			directions.push_back(side ? 1 : 3);
			directions.push_back(side ? 3 : 1);
		}
		if (dy == 0) {
			directions.push_back(side ? 0 : 2);
			directions.push_back(side ? 2 : 0);
		}
	}
	for (const auto direction : directions) {
		grid.facing = direction;
		animation.state = direction;
		if (step(entity, grid, direction, grid.reactStep)) {
			break;
		}
		// Walking into the other one : it touched it, no other way
		if (grid.touched == grid.reactTo) {
			break;
		}
	}
	// It charges without stopping ; it runs away hesitating (the player can catch it)
	grid.wait = grid.reaction == GridMovementComponent::Reaction::Approach ? 40u :
		static_cast<unsigned int>(std::uniform_int_distribution<int>{ 80, 320 }(m_random));
	return true;
}

void GridMovementSystem::startAutomaticStep(ska::EntityId entity, GridMovementComponent& grid, unsigned int ellapsedTime, bool walking) {
	// Towards or away from a character in sight
	if (react(entity, grid, ellapsedTime)) {
		return;
	}
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
