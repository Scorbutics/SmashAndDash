#include <fstream>
#include <algorithm>
#include <cmath>
#include "Game/__internalConfig/LoggerConfig.h"
#include "ChunkEntities.h"
#include "ChunkOwnedComponent.h"
#include "WorldState.h"
#include "Chunks/DespawnRules.h"
#include "../CustomEntityManager.h"
#include "../Control/GridMovementComponent.h"
#include "../Mobs/ArrivalComponent.h"
#include "../Mobs/WildPokemonComponent.h"
#include "../Fight/FightComponent.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"

namespace {
	constexpr auto FileName = "/Chunks.txt";
}

ChunkEntities::ChunkEntities(CustomEntityManager& entityManager, WorldState& worldState) :
	m_entityManager(entityManager),
	m_worldState(worldState) {
}

void ChunkEntities::load(const std::string& saveFolder) {
	auto file = std::ifstream{ saveFolder + FileName };
	m_store = chunks::EntityStore{};
	if (file) {
		m_store.read(file);
	}
}

void ChunkEntities::save(const std::string& saveFolder, const std::vector<std::pair<std::string, ska::Rectangle>>& loaded) const {
	auto store = m_store;
	for (const auto& [name, blocks] : loaded) {
		store.chunks[name] = record(capture(blocks));
	}
	auto file = std::ofstream{ saveFolder + FileName, std::ios::trunc };
	store.write(file);
}

std::vector<std::pair<ska::EntityId, chunks::EntityRecord>> ChunkEntities::capture(const ska::Rectangle& blocks) const {
	auto result = std::vector<std::pair<ska::EntityId, chunks::EntityRecord>>{};
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	// The block of a position of an entity (the corner of its block), when it is in the chunk
	const auto place = [&](float x, float y, chunks::EntityRecord& e) {
		const auto bx = static_cast<int>(std::floor((x + blockSize / 2.F) / blockSize));
		const auto by = static_cast<int>(std::floor((y + blockSize / 2.F) / blockSize));
		e.x = bx - blocks.x;
		e.y = by - blocks.y;
		return e.x >= 0 && e.y >= 0 && e.x < blocks.w && e.y < blocks.h;
	};
	// By id : the same order every time
	auto entities = std::vector<ska::EntityId>{ m_entityManager.entities().begin(), m_entityManager.entities().end() };
	std::sort(entities.begin(), entities.end());
	for (const auto entity : entities) {
		auto e = chunks::EntityRecord{};
		if (m_entityManager.hasComponent<ArrivalComponent>(entity)) {
			// On its way : it is there once back (one flying away is gone)
			const auto& arrival = m_entityManager.getComponent<ArrivalComponent>(entity);
			if (!place(arrival.to.x, arrival.to.y, e)) {
				continue;
			}
			if (arrival.kind != ArrivalComponent::Kind::TakeOff) {
				e.monster = arrival.monster;
				e.level = arrival.level;
				e.attitude = arrival.attitude;
				e.pathType = arrival.pathType;
				e.stay = arrival.stay.value_or(-1);
			}
			result.emplace_back(entity, std::move(e));
			continue;
		}
		if (!m_entityManager.hasComponent<ska::PositionComponent>(entity)) {
			continue;
		}
		const auto wild = m_entityManager.hasComponent<WildPokemonComponent>(entity) && m_entityManager.hasComponent<FightComponent>(entity);
		const auto character = m_entityManager.hasComponent<ChunkOwnedComponent>(entity);
		if (!wild && !character) {
			continue;
		}
		const auto& position = m_entityManager.getComponent<ska::PositionComponent>(entity);
		if (!place(static_cast<float>(position.x), static_cast<float>(position.y), e)) {
			continue;
		}
		if (m_entityManager.hasComponent<ska::AnimationComponent>(entity)) {
			e.facing = static_cast<int>(m_entityManager.getComponent<ska::AnimationComponent>(entity).state);
		}
		if (wild) {
			const auto& fight = m_entityManager.getComponent<FightComponent>(entity);
			const auto& component = m_entityManager.getComponent<WildPokemonComponent>(entity);
			e.monster = static_cast<int>(fight.opponentScriptId);
			e.level = static_cast<int>(fight.level);
			e.attitude = AttitudeName(component.attitude);
			e.pathType = m_entityManager.hasComponent<GridMovementComponent>(entity) ? 1 : 0;
			e.stay = component.stay.value_or(-1);
			e.calm = component.calm;
		} else {
			const auto& owned = m_entityManager.getComponent<ChunkOwnedComponent>(entity);
			e.kind = chunks::EntityRecord::Kind::Character;
			e.sprite = owned.sprite;
			e.name = owned.name;
			e.script = owned.script;
			e.arguments = owned.arguments;
			e.anchored = owned.anchored;
			e.animatedWhenStill = owned.animatedWhenStill;
			e.layer = static_cast<int>(position.layer);
			if (m_entityManager.hasComponent<GridMovementComponent>(entity)) {
				const auto& grid = m_entityManager.getComponent<GridMovementComponent>(entity);
				e.movement = grid.mode == GridMovementComponent::Mode::Wander ? "wander" : (grid.mode == GridMovementComponent::Mode::Path ? "path" : "");
				e.path = grid.path;
			}
		}
		result.emplace_back(entity, std::move(e));
	}
	return result;
}

chunks::ChunkRecord ChunkEntities::record(const std::vector<std::pair<ska::EntityId, chunks::EntityRecord>>& entities) const {
	auto result = chunks::ChunkRecord{};
	result.left = m_store.time;
	auto wilds = 0;
	for (const auto& [entity, e] : entities) {
		if (e.kind == chunks::EntityRecord::Kind::Wild) {
			// (A drawing flying away has no Pokemon)
			if (e.monster == 0 || ++wilds > chunks::ChunkCap) {
				continue;
			}
		}
		result.entities.push_back(e);
	}
	return result;
}

void ChunkEntities::leave(const std::string& name, const ska::Rectangle& blocks) {
	const auto entities = capture(blocks);
	m_store.chunks[name] = record(entities);
	for (const auto& [entity, e] : entities) {
		m_worldState.removeCharacter(entity);
	}
	if (!entities.empty()) {
		SLOG_STATIC(ska::LogLevel::Info, ChunkEntities) << "Chunk " << name << " : " << m_store.chunks[name].entities.size() << " entities kept";
	}
}

void ChunkEntities::arrive(const std::string& name, const ska::Rectangle& blocks) {
	const auto found = m_store.chunks.find(name);
	if (found == m_store.chunks.end()) {
		// The first time : its loading events make its entities. Known from now on
		m_store.chunks[name].left = m_store.time;
		return;
	}
	const auto forgotten = chunks::Forgotten(m_store.time, found->second.left);
	// The entities are on the map now, the record stays (the chunk is known)
	auto entities = std::move(found->second.entities);
	found->second.entities.clear();
	auto restored = 0;
	for (const auto& e : entities) {
		if (e.kind == chunks::EntityRecord::Kind::Wild && forgotten) {
			continue;
		}
		restore(e, { blocks.x + e.x, blocks.y + e.y });
		restored++;
	}
	if (!entities.empty()) {
		SLOG_STATIC(ska::LogLevel::Info, ChunkEntities) << "Chunk " << name << " : " << restored << " of " << entities.size() << " entities back";
	}
}

void ChunkEntities::restore(const chunks::EntityRecord& e, ska::Point<int> block) {
	if (e.kind == chunks::EntityRecord::Kind::Wild) {
		// (a swimmer back in its water : it stays there, as a Pokemon on land stays on land)
		if (!m_worldState.freeBlock(block.x, block.y, m_worldState.waterBlock(block.x, block.y)) && !m_worldState.waterBlock(block.x, block.y)) {
			const auto blockSize = static_cast<int>(m_worldState.getWorld().getBlockSize());
			const auto target = m_worldState.getWorld().getCollisionProfile().placeOnNearestPracticableBlock(1,
				ska::Rectangle{ block.x * blockSize, block.y * blockSize, blockSize, blockSize }, 2);
			block = { target.x / blockSize, target.y / blockSize };
		}
		const auto wild = m_worldState.createWild(e.monster, e.level, e.pathType, e.attitude, block,
			e.stay < 0 ? std::nullopt : std::optional<int>{ e.stay });
		m_entityManager.getComponent<WildPokemonComponent>(wild).calm = e.calm;
		m_entityManager.getComponent<ska::AnimationComponent>(wild).state = e.facing;
		return;
	}
	const auto character = m_worldState.addCharacter(e.sprite, block, e.name, e.script, e.arguments);
	m_entityManager.getComponent<ska::AnimationComponent>(character).state = e.facing;
	m_entityManager.getComponent<ska::PositionComponent>(character).layer = static_cast<std::size_t>(std::max(0, e.layer));
	m_entityManager.getComponent<ChunkOwnedComponent>(character).animatedWhenStill = e.animatedWhenStill;
	m_entityManager.getComponent<ska::AnimationComponent>(character).animatedWhenStill = e.animatedWhenStill;
	if (e.anchored) {
		m_worldState.anchor(character);
	}
	if (!e.movement.empty()) {
		auto grid = GridMovementComponent{};
		grid.mode = e.movement == "path" ? GridMovementComponent::Mode::Path : GridMovementComponent::Mode::Wander;
		grid.path = e.path;
		grid.wait = 1000;
		grid.facing = e.facing;
		if (grid.mode != GridMovementComponent::Mode::Path || !grid.path.empty()) {
			m_entityManager.addComponent<GridMovementComponent>(character, std::move(grid));
		}
	}
}
