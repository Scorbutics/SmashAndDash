#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleCameraSystem.h"
#include "../../World/WorldState.h"
#include <algorithm>
#include <cmath>
#include "Graphic/System/CameraSystem.h"

BattleCameraSystem::BattleCameraSystem(ska::EntityManager& em, ska::CameraSystem& camera, BattleStage& stage) :
	System(em),
	m_entities(em),
	m_camera(camera),
	m_stage(stage) {
}

ska::Point<float> BattleCameraSystem::center(ska::EntityId entity) {
	return ska::PositionComponent::getCenterPosition(m_componentAccessor.get<ska::PositionComponent>(entity), m_componentAccessor.get<ska::HitboxComponent>(entity));
}

void BattleCameraSystem::refresh(unsigned int ellapsedTime) {
	const auto phase = m_stage.phase();
	const auto& processed = getEntities();
	if (phase == BattlePhase::Intro || phase == BattlePhase::Over || processed.find(m_stage.player) == processed.end()) {
		return;
	}
	m_time += ellapsedTime;
	const auto seconds = static_cast<float>(ellapsedTime) / 1000.F;

	// The camera of the battle starts where the camera is (no jump) and pushes in
	if (m_strategy == nullptr) {
		auto strategy = std::make_unique<BattleCameraStrategy>();
		m_strategy = strategy.get();
		const auto& display = m_camera.getDisplay();
		m_focus = ska::Point<float>{ static_cast<float>(display.x + display.w / 2), static_cast<float>(display.y + display.h / 2) };
		m_zoom = m_camera.getZoom();
		m_strategy->point = m_focus;
		m_camera.changeStrategy(std::move(strategy));
	}

	// The Pokemon of the player, its opponents still standing
	const auto player = center(m_stage.player);
	auto enemies = ska::Point<float>{};
	auto enemyCount = 0;
	auto low = player;
	auto high = player;
	for (const auto entity : processed) {
		const auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
		if (fighter.knockedOut && phase == BattlePhase::Fight) {
			continue;
		}
		const auto position = center(entity);
		low = ska::Point<float>{ std::min(low.x, position.x), std::min(low.y, position.y) };
		high = ska::Point<float>{ std::max(high.x, position.x), std::max(high.y, position.y) };
		if (fighter.team != 0) {
			enemies = ska::Point<float>{ enemies.x + position.x, enemies.y + position.y };
			enemyCount++;
		}
	}
	// The trainer, who follows the fight from behind its Pokemon (TrainerCoachSystem), stays in sight too (its head)
	if (phase == BattlePhase::Fight && m_stage.trainer.has_value() && m_entities.hasComponent<ska::PositionComponent>(*m_stage.trainer) &&
		m_entities.hasComponent<ska::HitboxComponent>(*m_stage.trainer)) {
		const auto trainer = ska::PositionComponent::getCenterPosition(m_entities.getComponent<ska::PositionComponent>(*m_stage.trainer), m_entities.getComponent<ska::HitboxComponent>(*m_stage.trainer));
		low = ska::Point<float>{ std::min(low.x, trainer.x), std::min(low.y, trainer.y - 24.F) };
		high = ska::Point<float>{ std::max(high.x, trainer.x), std::max(high.y, trainer.y) };
	}
	if (enemyCount > 0) {
		enemies = ska::Point<float>{ enemies.x / static_cast<float>(enemyCount), enemies.y / static_cast<float>(enemyCount) };
	} else {
		enemies = player;
	}

	// Its point : towards the Pokemon of the player (the opponents stay in sight), a slow drift
	auto target = ska::Point<float>{ player.x + (enemies.x - player.x) * 0.4F, player.y + (enemies.y - player.y) * 0.4F };
	const auto t = static_cast<float>(m_time);
	target.x += 14.F * std::sin(t / 1700.F);
	target.y += 9.F * std::cos(t / 2300.F);

	// The zoom : every fighter in sight (with a margin), closer when they are close
	const auto screen = m_camera.getScreenSize();
	const auto halfWidth = std::max(std::abs(high.x - target.x), std::abs(low.x - target.x)) + 120.F;
	const auto halfHeight = std::max(std::abs(high.y - target.y), std::abs(low.y - target.y)) + 110.F;
	auto zoom = std::min(static_cast<float>(screen.x) / (2.F * halfWidth), static_cast<float>(screen.y) / (2.F * halfHeight));
	zoom += 0.02F * std::sin(t / 2600.F);

	// A Pokemon prepares a long attack : the camera looks at it, a little closer
	for (const auto entity : processed) {
		const auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
		if (!fighter.knockedOut && fighter.windUp > 0 && fighter.windUpDuration >= 400) {
			const auto caster = center(entity);
			target = ska::Point<float>{ target.x + (caster.x - target.x) * 0.25F, target.y + (caster.y - target.y) * 0.25F };
			zoom += 0.08F;
			break;
		}
	}

	// A hit (the start of a hit stop) : a push in
	if (m_stage.frozen() && !m_wasFrozen) {
		m_kick = std::min(0.16F, m_kick + 0.07F);
	}
	m_wasFrozen = m_stage.frozen();
	m_kick *= std::exp(-seconds * 5.F);

	auto speed = 3.F;
	switch (phase) {
	case BattlePhase::KnockOut: {
		// Closer on the last Pokemon K.O.
		auto lastKnockedOut = ska::EntityId{ 0 };
		auto latest = static_cast<unsigned int>(-1);
		for (const auto entity : processed) {
			const auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
			if (fighter.knockedOut && fighter.knockedOutTime < latest) {
				latest = fighter.knockedOutTime;
				lastKnockedOut = entity;
			}
		}
		if (lastKnockedOut != 0) {
			target = center(lastKnockedOut);
			zoom = 1.9F;
			speed = 1.6F;
		}
		break;
	}
	case BattlePhase::Fled:
		zoom = WorldState::MapZoom;
		speed = 2.F;
		break;
	case BattlePhase::Outro:
		// Back to the trainer, as the map shows it : the map goes on from there
		if (m_stage.trainer.has_value() && m_entities.hasComponent<ska::PositionComponent>(*m_stage.trainer) && m_entities.hasComponent<ska::HitboxComponent>(*m_stage.trainer)) {
			target = ska::PositionComponent::getCenterPosition(m_entities.getComponent<ska::PositionComponent>(*m_stage.trainer), m_entities.getComponent<ska::HitboxComponent>(*m_stage.trainer));
		}
		zoom = WorldState::MapZoom;
		speed = 2.4F;
		break;
	default:
		// The push in of the start of the fight is slower
		speed = m_stage.phaseTime() < 1200 ? 1.6F : 3.F;
		break;
	}
	zoom = phase == BattlePhase::Outro ? zoom : std::max(MinZoom, std::min(MaxZoom + 0.3F, zoom)) + m_kick;
	// The bond unleashes a skill (BondSystem) : close on the Pokemon, until its skill goes
	const auto& cinematic = m_stage.cinematic;
	if (cinematic.active && !cinematic.released && phase == BattlePhase::Fight && m_entities.hasComponent<ska::PositionComponent>(cinematic.user) &&
		m_entities.hasComponent<ska::HitboxComponent>(cinematic.user)) {
		const auto user = center(cinematic.user);
		target = ska::Point<float>{ user.x, user.y - 8.F };
		zoom = MaxZoom + 0.6F;
		speed = 6.F;
	}
	// A clash (ClashSystem) : it dives on the two Pokemon, the map breaks around them (then the battleground hides the
	// world), it stays there until the battleground goes away
	const auto& turn = m_stage.turn;
	if (turn.active() && turn.step != BattleTurn::Step::Return && phase == BattlePhase::Fight) {
		const auto impact = turn.step == BattleTurn::Step::Impact;
		target = turn.center;
		zoom = impact ? m_zoom + 0.25F : MaxZoom + 1.1F;
		speed = impact ? 14.F : 5.F;
	}

	// Smoothly : the point and the zoom follow their targets
	const auto follow = 1.F - std::exp(-seconds * speed);
	m_focus = ska::Point<float>{ m_focus.x + (target.x - m_focus.x) * follow, m_focus.y + (target.y - m_focus.y) * follow };
	m_zoom += (zoom - m_zoom) * (1.F - std::exp(-seconds * speed * 1.3F));
	m_strategy->point = m_focus;
	m_camera.setZoom(m_zoom);
}
