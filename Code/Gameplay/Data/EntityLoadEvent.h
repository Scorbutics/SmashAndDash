#pragma once
#include "../../ska/Inputs/Readers/IniReader.h"
#include "Statistics.h"
#include "MonsterDescription.h"

struct EntityLoadEvent {
	EntityLoadEvent(ska::IniReader& reader, unsigned int pkmnid, unsigned int hp) :
	m_stats(std::unique_ptr<Statistics>(new Statistics(&reader, "BaseStats"))),
		m_description(std::unique_ptr<const MonsterDescription>(new MonsterDescription(reader, "Description"))),
		baseExpNeeded(reader.get<unsigned int>("Experience level_1_exp_needed")),
		baseExpDropped(reader.get<unsigned int>("Experience level_1_exp_dropped")),
		expEvolutionType(reader.get<std::string>("Experience type")),
		currentHp(hp),
		id(pkmnid),
		stats(m_stats.get()),
		description(m_description.get()) {

	}

	void exportStatistics(std::unique_ptr<Statistics>* destination) {
		*destination = std::move(m_stats);
		stats = destination->get();
	}

	void exportDescription(std::unique_ptr<const MonsterDescription>* destination) {
		*destination = std::move(m_description);
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
