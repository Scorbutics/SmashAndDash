#pragma once
#include <string>
#include <vector>
#include "Core/Point.h"
#include "Core/Rectangle.h"
#include "Core/ECS/ECSDefines.h"
#include "Chunks/EntityRecords.h"

class CustomEntityManager;
class WorldState;

// The entities of the chunks : taken with their chunk when it leaves (their records kept, <save>/Chunks.txt), brought
// back when it comes again. The wild Pokemon, the arrivals on their way, the characters of the maps.
class ChunkEntities {
public:
	ChunkEntities(CustomEntityManager& entityManager, WorldState& worldState);

	// The records of the save (the folder of the save)
	void load(const std::string& saveFolder);
	// The records, with the entities of the chunks loaded (their blocks in the world), to the save
	void save(const std::string& saveFolder, const std::vector<std::pair<std::string, ska::Rectangle>>& loaded) const;

	// The chunk ran its loading events once (it has a record) : they do not run again
	bool known(const std::string& name) const { return m_store.chunks.count(name) != 0; }
	// A chunk comes : its entities back (blocks : its blocks in the world)
	void arrive(const std::string& name, const ska::Rectangle& blocks);
	// A chunk leaves : its entities in its record, removed from the map
	void leave(const std::string& name, const ska::Rectangle& blocks);

	// The time played, which the records are dated with
	void tick(unsigned int ellapsedTime) { m_store.time += ellapsedTime; }

private:
	// The entities of the map standing in the blocks, with their records
	std::vector<std::pair<ska::EntityId, chunks::EntityRecord>> capture(const ska::Rectangle& blocks) const;
	chunks::ChunkRecord record(const std::vector<std::pair<ska::EntityId, chunks::EntityRecord>>& entities) const;
	void restore(const chunks::EntityRecord& record, ska::Point<int> block);

	CustomEntityManager& m_entityManager;
	WorldState& m_worldState;
	chunks::EntityStore m_store;
};
