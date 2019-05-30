#pragma once
#include <memory>

#include "Inputs/Readers/IniReader.h"
#include "Statistics.h"
#include "MonsterDescription.h"


struct EntityLoadEvent {
	EntityLoadEvent(ska::IniReader& reader, unsigned int pkmnid, unsigned int hp) :
	m_stats(std::make_unique<Statistics>(&reader, "BaseStats")),
		m_description(std::make_unique<MonsterDescription const>(reader, "Description")),
		stats(m_stats.get()),
		description(m_description.get()),
		id(pkmnid),
		baseExpNeeded(reader.get<unsigned int>("Experience level_1_exp_needed")),
		baseExpDropped(reader.get<unsigned int>("Experience level_1_exp_dropped")),
		expEvolutionType(reader.get<std::string>("Experience type")),
		currentHp(hp) {

	}

	EntityLoadEvent(const EntityLoadEvent&) = delete;
	EntityLoadEvent& operator=(const EntityLoadEvent&) = delete;

	void exportStatistics(std::unique_ptr<Statistics>* destination) {
		*destination = move(m_stats);
		stats = destination->get();
	}

	void exportDescription(std::unique_ptr<const MonsterDescription>* destination) {
		*destination = move(m_description);
		description = destination->get();
	}

	~EntityLoadEvent() = default;

private:
	std::unique_ptr<Statistics> m_stats;
	std::unique_ptr<const MonsterDescription> m_description;

public:
	Statistics* stats;
	const MonsterDescription* description;

	const unsigned int id;
	const unsigned int baseExpNeeded;
	const unsigned int baseExpDropped;
	const std::string expEvolutionType;
	unsigned int currentHp;

};
