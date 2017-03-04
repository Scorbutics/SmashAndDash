#pragma once
#include "../../ska/Inputs/Readers/IniReader.h"

struct MonsterDescription {
	MonsterDescription(ska::IniReader& reader, const std::string&) : 
		name(reader.get<std::string>("Description name")),
		type1(reader.get<std::string>("Description type1")),
		type2(reader.get<std::string>("Description type2")),
		pokedex(reader.get<std::string>("Description pokedex")),
		captureRate(reader.get<unsigned int>("Description capture_rate")) {
	}

	void operator=(const MonsterDescription&) = delete;

	const std::string name;
	const std::string type1;
	const std::string type2;
	const std::string pokedex;
	const unsigned int captureRate;

};
