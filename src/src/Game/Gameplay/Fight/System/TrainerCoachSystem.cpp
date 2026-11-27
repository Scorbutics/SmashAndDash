#include "Game/__internalConfig/LoggerConfig.h"
#include "TrainerCoachSystem.h"
#include <algorithm>
#include <cmath>
#include "Base/IO/Readers/IniReader.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "Core/Utils/TimeUtils.h"
#include "World/TileWorld.h"
#include "../BattleRules.h"
#include "../SkillsHolderComponent.h"
#include "../../CustomEntityManager.h"
#include "../../World/WorldState.h"

namespace {
	// The direction of a character sheet facing a vector (0 : down, 1 : right, 2 : up, 3 : left)
	int Facing(float dx, float dy) {
		return std::abs(dx) > std::abs(dy) ? (dx > 0.F ? 1 : 3) : (dy > 0.F ? 0 : 2);
	}

	ska::Point<float> Rotate(ska::Point<float> v, float angle) {
		const auto c = std::cos(angle);
		const auto s = std::sin(angle);
		return { v.x * c - v.y * s, v.x * s + v.y * c };
	}

	// How long a line stays in its balloon (real time) : long enough to be read
	unsigned int SpeechDuration(const std::string& text) {
		return std::min(3200U, 1300U + 45U * static_cast<unsigned int>(text.size()));
	}
}

TrainerCoachSystem::TrainerCoachSystem(ska::EntityManager& em, WorldState& worldState, BattleStage& stage) :
	System(em),
	m_entities(em),
	m_worldState(worldState),
	m_stage(stage),
	m_rules(worldState.getBattleRules()) {
}

ska::Point<float> TrainerCoachSystem::center(ska::EntityId entity) const {
	return ska::PositionComponent::getCenterPosition(m_entities.getComponent<ska::PositionComponent>(entity), m_entities.getComponent<ska::HitboxComponent>(entity));
}

bool TrainerCoachSystem::standing(ska::EntityId entity) const {
	const auto& processed = getEntities();
	if (processed.find(entity) == processed.end()) {
		return false;
	}
	const auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
	return !fighter.knockedOut && !fighter.inBall && m_componentAccessor.get<BattleComponent>(entity).hp > 0;
}

bool TrainerCoachSystem::walkable(ska::Point<float> point) {
	auto& world = m_worldState.getWorld();
	const auto blockSize = static_cast<float>(world.getBlockSize());
	if (point.x < 0.F || point.y < 0.F) {
		return false;
	}
	const auto x = static_cast<std::size_t>(point.x / blockSize);
	const auto y = static_cast<std::size_t>(point.y / blockSize);
	if (x >= world.getBlocksX() || y >= world.getBlocksY()) {
		return false;
	}
	const auto& collisions = world.getCollisionProfile();
	if (collisions.collide(1, x, y)) {
		return false;
	}
	const auto* ground = collisions.getBlock(0, x, y);
	return ground == nullptr || (ground->properties.bitMask & ska::TilePropertiesType::TILE_PROPERTY_WATER) == 0;
}

std::optional<ska::Point<float>> TrainerCoachSystem::plan(ska::EntityId trainer) {
	const auto player = m_stage.player;
	if (!standing(player)) {
		return std::nullopt;
	}
	const auto opponent = m_componentAccessor.get<FighterComponent>(player).opponent;
	if (!standing(opponent)) {
		return std::nullopt;
	}
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	const auto pokemon = center(player);
	const auto enemy = center(opponent);
	auto away = ska::Point<float>{ pokemon.x - enemy.x, pokemon.y - enemy.y };
	const auto length = std::hypot(away.x, away.y);
	away = length > 1.F ? ska::Point<float>{ away.x / length, away.y / length } : ska::Point<float>{ 0.F, 1.F };

	// In the arena (it stays on the screen), half a block from its edges
	const auto& arena = m_stage.arena;
	const auto margin = blockSize * 0.6F;
	const auto clampToArena = [&](ska::Point<float> p) {
		if (arena.w <= 0 || arena.h <= 0) {
			return p;
		}
		return ska::Point<float>{ std::clamp(p.x, static_cast<float>(arena.x) + margin, static_cast<float>(arena.x + arena.w) - margin),
			std::clamp(p.y, static_cast<float>(arena.y) + margin, static_cast<float>(arena.y + arena.h) - margin) };
	};
	// Right behind its Pokemon, else a bit on a side
	const auto here = center(trainer);
	// (Pushed against an edge of the arena : on a side, not on its Pokemon ; between the fighters only when nothing else
	// is free)
	const auto enemyDistance = std::hypot(pokemon.x - enemy.x, pokemon.y - enemy.y);
	for (const auto behindOnly : { true, false }) {
		for (const auto angle : { 0.F, 0.8F, -0.8F, 1.5F, -1.5F, 2.2F, -2.2F }) {
			const auto direction = Rotate(away, angle);
			const auto spot = clampToArena(ska::Point<float>{ pokemon.x + direction.x * Behind * blockSize, pokemon.y + direction.y * Behind * blockSize });
			const auto fromEnemy = std::hypot(spot.x - enemy.x, spot.y - enemy.y);
			if (fromEnemy < KeepAway * blockSize || std::hypot(spot.x - pokemon.x, spot.y - pokemon.y) < Clear * blockSize ||
				(behindOnly && fromEnemy < enemyDistance) || !walkable(spot)) {
				continue;
			}
			// Where it already is is good enough : it does not walk for nothing
			if (std::hypot(spot.x - here.x, spot.y - here.y) < blockSize * 0.9F) {
				return std::nullopt;
			}
			return spot;
		}
	}
	return std::nullopt;
}

void TrainerCoachSystem::walk(ska::EntityId trainer, unsigned int ellapsedTime) {
	auto* animation = m_entities.hasComponent<ska::AnimationComponent>(trainer) ? &m_entities.getComponent<ska::AnimationComponent>(trainer) : nullptr;
	const auto here = center(trainer);
	auto moving = false;
	auto facing = ska::Point<float>{};
	if (m_target.has_value()) {
		const auto dx = m_target->x - here.x;
		const auto dy = m_target->y - here.y;
		const auto distance = std::hypot(dx, dy);
		const auto step = std::min(distance, Speed * static_cast<float>(ellapsedTime) / 1000.F);
		const auto next = ska::Point<float>{ here.x + dx / std::max(distance, 0.001F) * step, here.y + dy / std::max(distance, 0.001F) * step };
		if (distance < 2.F || !walkable(next)) {
			m_target.reset();
		} else {
			const auto& pc = m_entities.getComponent<ska::PositionComponent>(trainer);
			if (!m_exact.has_value() || std::abs(m_exact->x - static_cast<float>(pc.x)) >= 1.F || std::abs(m_exact->y - static_cast<float>(pc.y)) >= 1.F) {
				m_exact = ska::Point<float>{ static_cast<float>(pc.x), static_cast<float>(pc.y) };
			}
			m_exact = ska::Point<float>{ m_exact->x + next.x - here.x, m_exact->y + next.y - here.y };
			CustomEntityManager::place(m_entities, m_worldState.getSpace(), trainer, *m_exact);
			moving = true;
			facing = { dx, dy };
		}
	}
	if (animation == nullptr) {
		return;
	}
	// Still : it faces its Pokemon
	if (!moving && m_stage.phase() == BattlePhase::Fight && standing(m_stage.player)) {
		const auto pokemon = center(m_stage.player);
		facing = { pokemon.x - here.x, pokemon.y - here.y };
	}
	animation->drivenUntil = ska::TimeUtils::getTicks() + 150;
	animation->walking = moving;
	animation->walkDelay = 160;
	if (facing.x != 0.F || facing.y != 0.F) {
		animation->state = Facing(facing.x, facing.y);
	}
}

void TrainerCoachSystem::comment(const std::string& event, const std::string& detail) {
	if (m_spoke && m_quiet < m_rules.linePause(event)) {
		return;
	}
	const auto roll = ska::NumberUtils::random(0, 99);
	const auto pick = ska::NumberUtils::random(0, 999);
	auto text = m_rules.line(event, monsterName(), detail, roll, pick);
	if (text.empty()) {
		return;
	}
	SLOG(ska::LogLevel::Info) << "Battle : the trainer says \"" << text << "\" (" << event << ")";
	const auto duration = SpeechDuration(text);
	m_stage.say(std::move(text), duration);
	m_quiet = 0;
	m_spoke = true;
}

const std::string& TrainerCoachSystem::monsterName() {
	if (m_name.empty() && m_entities.hasComponent<BattleComponent>(m_stage.player)) {
		const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(m_entities.getComponent<BattleComponent>(m_stage.player).monster) + ".ini" };
		m_name = data.exists("Description name") ? data.get<std::string>("Description name") : "?";
	}
	return m_name;
}

void TrainerCoachSystem::watch() {
	const auto phase = m_stage.phase();
	const auto changed = phase != m_phase;
	m_phase = phase;
	if (changed && phase == BattlePhase::Fight) {
		comment("start");
	}
	if (changed && phase == BattlePhase::KnockOut) {
		comment(m_stage.winnerTeam == 0 ? "knockout" : "fainted");
	}
	// The cues of the hits and of the items
	for (const auto& cue : m_stage.takeCues()) {
		if (phase == BattlePhase::Fight) {
			comment(cue.event, cue.detail);
		}
	}
	if (phase != BattlePhase::Fight || !standing(m_stage.player)) {
		return;
	}
	const auto& fighter = m_componentAccessor.get<FighterComponent>(m_stage.player);
	// A skill used (the time of its last use changed)
	for (auto i = std::size_t{ 0 }; i < fighter.lastUses.size(); i++) {
		if (fighter.lastUses[i] != m_lastUses[i]) {
			m_lastUses[i] = fighter.lastUses[i];
			if (m_entities.hasComponent<SkillsHolderComponent>(m_stage.player)) {
				comment("attack", m_entities.getComponent<SkillsHolderComponent>(m_stage.player).skills[i].name);
			}
		}
	}
	const auto dashing = fighter.dash > 0;
	if (dashing && !m_dashing) {
		comment("dodge");
	}
	m_dashing = dashing;
	const auto& bc = m_componentAccessor.get<BattleComponent>(m_stage.player);
	if (!m_low && bc.hp > 0 && bc.hp * 4 <= bc.maxHp) {
		m_low = true;
		comment("low");
	}
}

void TrainerCoachSystem::refresh(unsigned int ellapsedTime) {
	m_quiet += ellapsedTime;
	const auto trainer = m_stage.trainer;
	if (!trainer.has_value() || !m_entities.hasComponent<ska::PositionComponent>(*trainer) || !m_entities.hasComponent<ska::HitboxComponent>(*trainer)) {
		m_stage.takeCues();
		return;
	}
	watch();
	// It walks during the fight only (the intro and the outro place it : the pokeball thrown, recalled)
	if (m_stage.phase() != BattlePhase::Fight) {
		m_target.reset();
		return;
	}
	m_planTime += ellapsedTime;
	if (m_planTime >= PlanPeriod) {
		m_planTime = 0;
		if (const auto spot = plan(*trainer); spot.has_value()) {
			m_target = spot;
		}
	}
	// Battle time : it freezes with the hit stops
	walk(*trainer, m_stage.delta());
}
