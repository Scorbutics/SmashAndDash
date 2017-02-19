#pragma once
#include "../../ska/Inputs/Readers/IniReader.h"
#include "Statistics.h"
#include "MonsterDescription.h"

struct EntityLoadEvent {
	EntityLoadEvent(ska::IniReader& reader, unsigned int pkmnid, unsigned int hp) :
		stats(&reader, "BaseStats"),
		description(reader, "Description"),
		baseExpNeeded(reader.get<unsigned int>("Experience level_1_exp_needed")),
		baseExpDropped(reader.get<unsigned int>("Experience level_1_exp_dropped")),
		expEvolutionType(reader.get<std::string>("Experience type")),
		currentHp(hp),
		id(pkmnid) {

	}

	Statistics stats;
	const MonsterDescription description;

	const unsigned int id;
	const unsigned int baseExpNeeded;
	const unsigned int baseExpDropped;
	const std::string expEvolutionType;
	unsigned int currentHp;

	~EntityLoadEvent() = default;

};
