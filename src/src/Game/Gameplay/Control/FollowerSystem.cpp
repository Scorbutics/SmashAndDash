#include "Game/__internalConfig/LoggerConfig.h"
#include "FollowerSystem.h"
#include <algorithm>
#include <cmath>
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Physic/CollidableComponent.h"
#include "Core/ECS/EntityLocator.h"
#include "Core/Utils/RectangleUtils.h"
#include "Core/Utils/TimeUtils.h"
#include "Graphic/GraphicComponent.h"
#include "../CustomEntityManager.h"
#include "../World/WorldState.h"
#include "../Data/Team.h"

FollowerSystem::FollowerSystem(CustomEntityManager& entityManager, WorldState& worldState) :
	System(entityManager),
	m_cem(entityManager),
	m_worldState(worldState) {
}

void FollowerSystem::dismiss(CustomEntityManager& entityManager, WorldState& worldState) {
	if (const auto* follower = worldState.getEntityLocator().getEntityId(Name); follower != nullptr) {
		entityManager.removeEntity(*follower);
	}
}

std::optional<int> FollowerSystem::lead() {
	const auto team = Team{ m_worldState.getSaveGame().getPathName() };
	const auto index = team.firstAble();
	if (!index.has_value()) {
		return {};
	}
	const auto member = team.member(*index);
	return member.has_value() ? std::optional<int>{ member->id } : std::nullopt;
}

void FollowerSystem::restart(const ska::PositionComponent& player) {
	m_trail.clear();
	m_trail.push_back(TrailPoint{ static_cast<float>(player.x), static_cast<float>(player.y), 0 });
}

std::optional<FollowerSystem::TrailPoint> FollowerSystem::behind() {
	// From the player back along its trail
	if (m_trail.size() < 2) {
		return {};
	}
	auto walked = 0.F;
	for (auto i = m_trail.size() - 1; i > 0; i--) {
		const auto& a = m_trail[i];
		const auto& b = m_trail[i - 1];
		const auto length = std::hypot(a.x - b.x, a.y - b.y);
		if (walked + length >= Distance) {
			const auto t = length > 0.F ? (Distance - walked) / length : 0.F;
			// The older points are not needed any more
			m_trail.erase(m_trail.begin(), m_trail.begin() + static_cast<long>(i - 1));
			return TrailPoint{ a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, t < 0.5F ? a.z : b.z };
		}
		walked += length;
	}
	return {};
}

ska::EntityId FollowerSystem::make(int species, const TrailPoint& at) {
	const auto blockSize = m_worldState.getWorld().getBlockSize();
	const auto follower = m_cem.createCharacter(ska::Point<int>{ 0, 0 }, species, static_cast<unsigned int>(blockSize), Name);
	auto& position = m_cem.getComponent<ska::PositionComponent>(follower);
	position.x = static_cast<long>(std::lround(at.x));
	position.y = static_cast<long>(std::lround(at.y));
	position.z = at.z;
	// A drawing only : it blocks nothing (no physics body), it is sorted by its exact position
	m_cem.removeComponent<ska::CollidableComponent>(follower);
	m_cem.getComponent<ska::GraphicComponent>(follower).sortStep = 0;
	m_appear = 0;
	m_still = 0;
	m_last = at;
	SLOG(ska::LogLevel::Info) << "Follower : pokemon " << species << " comes out";
	return follower;
}

void FollowerSystem::animate(ska::EntityId follower, const TrailPoint& at, const ska::PositionComponent& player, unsigned int ellapsedTime) {
	auto& position = m_cem.getComponent<ska::PositionComponent>(follower);
	position.x = static_cast<long>(std::lround(at.x));
	position.y = static_cast<long>(std::lround(at.y));
	position.z = at.z;
	if (m_cem.hasComponent<ska::MovementComponent>(follower)) {
		m_cem.getComponent<ska::MovementComponent>(follower).vz = 0.F;
	}

	// Walked along the trail : its steps in the direction it goes ; still a while : it turns to the player
	const auto dx = at.x - m_last.x;
	const auto dy = at.y - m_last.y;
	const auto moved = std::hypot(dx, dy);
	m_last = at;
	auto& animation = m_cem.getComponent<ska::AnimationComponent>(follower);
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	animation.drivenUntil = ska::TimeUtils::getTicks() + 200;
	animation.walking = moved > 0.2F;
	if (animation.walking) {
		m_still = 0;
		const auto speed = moved * 1000.F / static_cast<float>(std::max(1u, ellapsedTime));
		animation.walkDelay = std::max(60u, static_cast<unsigned int>(blockSize * 1000.F / std::max(speed, 60.F) / 2.F));
		animation.state = ska::RectangleUtils::getDirectionFromPos(ska::Point<float>{}, ska::Point<float>{ dx, dy });
	} else {
		m_still += ellapsedTime;
		if (m_still >= FaceAfter) {
			const auto toPlayer = ska::Point<float>{ static_cast<float>(player.x) - at.x, static_cast<float>(player.y) - at.y };
			if (std::abs(toPlayer.x) + std::abs(toPlayer.y) > 1.F) {
				animation.state = ska::RectangleUtils::getDirectionFromPos(ska::Point<float>{}, toPlayer);
			}
		}
	}

	// It comes out : a flash, its drawing growing from small (anchored at its feet)
	auto& graphic = m_cem.getComponent<ska::GraphicComponent>(follower);
	m_appear = std::min(AppearTime, m_appear + ellapsedTime);
	const auto t = static_cast<float>(m_appear) / static_cast<float>(AppearTime);
	const auto overshoot = 1.F + 0.12F * std::sin(t * 3.14159265F);
	const auto scale = t < 1.F ? (0.4F + 0.6F * t) * overshoot : 1.F;
	graphic.effect.anchorBottom = true;
	graphic.effect.scaleX = scale;
	graphic.effect.scaleY = scale;
	graphic.effect.flash = static_cast<unsigned char>(255.F * (1.F - t));
}

void FollowerSystem::refresh(unsigned int ellapsedTime) {
	const auto* playerId = m_worldState.getEntityLocator().getEntityId("trainer");
	if (playerId == nullptr || !m_cem.hasComponent<GridMovementComponent>(*playerId) || !m_cem.hasComponent<ska::PositionComponent>(*playerId)) {
		return;
	}
	const auto player = *playerId;
	const auto& position = m_cem.getComponent<ska::PositionComponent>(player);
	const auto* followerId = m_worldState.getEntityLocator().getEntityId(Name);
	auto follower = followerId == nullptr ? std::optional<ska::EntityId>{} : std::optional<ska::EntityId>{ *followerId };

	// Its Pokemon : the first one able to fight (read again from time to time)
	m_teamCheck += ellapsedTime;
	if (!m_checked || m_teamCheck >= TeamCheck) {
		m_teamCheck = 0;
		m_checked = true;
		const auto species = lead();
		if (species != m_species) {
			m_species = species;
			if (follower.has_value()) {
				// Another Pokemon : it comes out where the previous one was
				const auto& was = m_cem.getComponent<ska::PositionComponent>(*follower);
				const auto at = TrailPoint{ static_cast<float>(was.x), static_cast<float>(was.y), 0 };
				m_cem.removeEntity(*follower);
				follower = m_species.has_value() ? std::optional<ska::EntityId>{ make(*m_species, at) } : std::nullopt;
			}
		}
	}

	// Away : no Pokemon, the player surfs ; it comes back from the player
	const auto surfing = m_cem.getComponent<ska::AnimationComponent>(player).animatedWhenStill;
	if (!m_species.has_value() || surfing) {
		if (follower.has_value()) {
			m_cem.removeEntity(*follower);
		}
		restart(position);
		return;
	}

	// The trail : a jump of the player (a map change, a door, a teleport) starts it again
	const auto here = TrailPoint{ static_cast<float>(position.x), static_cast<float>(position.y), position.z };
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	if (m_trail.empty() || std::hypot(here.x - m_trail.back().x, here.y - m_trail.back().y) > blockSize * 1.5F) {
		restart(position);
	} else if (std::hypot(here.x - m_trail.back().x, here.y - m_trail.back().y) >= 0.5F || here.z != m_trail.back().z) {
		m_trail.push_back(here);
	}

	const auto at = behind();
	if (!follower.has_value()) {
		// Not there yet (a new map, after a battle, back from the water) : it comes out once the player walked away
		if (at.has_value()) {
			follower = make(*m_species, *at);
		} else {
			return;
		}
	}
	if (at.has_value()) {
		animate(*follower, *at, position, ellapsedTime);
	} else {
		animate(*follower, m_last, position, ellapsedTime);
	}
}
