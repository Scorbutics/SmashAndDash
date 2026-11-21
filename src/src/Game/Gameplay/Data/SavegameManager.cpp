#include "Game/__internalConfig/LoggerConfig.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "SavegameManager.h"
#include "../../Utils/ChargementImages.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Base/IO/Files/FileUtils.h"
#include "../../Gameplay/Data/Statistics.h"
#include "Logging/Logger.h"
#include "Core/ECS/EntityManager.h"

SavegameManager::SavegameManager(ska::EntityManager& em, PokemonGameEventDispatcher& ged, const std::string& filename):
	m_entityManager(em),
	m_ged(ged) {

	m_pathname = filename;
	loadTrainer();
}


void SavegameManager::newGame() {
	m_pathname = "new_game.ini";
}


int SavegameManager::getGameVariable(const std::string& x) const {
	const auto search = m_game_variables.find(x);
	if (search != m_game_variables.end()) {
		return search->second;
	}
	return 0;
}

void SavegameManager::setGameVariable(const std::string& x, const int value) {
		m_game_variables[x] = value;
}

void SavegameManager::saveGame(const std::string& mapName, int blockX, int blockY) {
	saveTrainer(mapName, blockX, blockY);
	saveVariables();
}

std::string SavegameManager::getSaveName() const {
	return m_pathname;
}

void SavegameManager::loadGame(const std::string& pathname) {
	m_pathname = pathname;
	loadTrainer();
	loadPokemonTeam();
	loadVariables();

	std::ofstream tmpScriptList(("./Data/Saves/" + m_pathname + "/tmpscripts.data").c_str(), std::ios::trunc);
}

// variables.ini : the [Variable] section, "name=value" (the numbered variables of the old scripts are named "%00001"...)
void SavegameManager::loadVariables() {
	m_game_variables.clear();
	auto file = std::ifstream{ "./Data/Saves/" + m_pathname + "/variables.ini" };
	auto line = std::string{};
	auto inVariables = false;
	while (std::getline(file, line)) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		if (!line.empty() && line.front() == '[') {
			inVariables = line == "[Variable]";
			continue;
		}
		const auto separator = line.find('=');
		if (!inVariables || separator == std::string::npos || separator == 0) {
			continue;
		}
		try {
			m_game_variables[line.substr(0, separator)] = std::stoi(line.substr(separator + 1));
		} catch (std::exception&) {
			SLOG(ska::LogLevel::Warn) << "Savegame : invalid variable line \"" << line << "\"";
		}
	}
}

void SavegameManager::saveVariables() const {
	auto names = std::vector<std::string>{};
	for (const auto& variable : m_game_variables) {
		names.push_back(variable.first);
	}
	std::sort(names.begin(), names.end());
	auto file = std::ofstream{ "./Data/Saves/" + m_pathname + "/variables.ini", std::ios::trunc };
	file << "[Game]\nvar_number=" << names.size() << "\n[Variable]\n";
	for (const auto& name : names) {
		file << name << "=" << m_game_variables.at(name) << "\n";
	}
}

void SavegameManager::loadPokemonTeam() {
	ska::IniReader pkmnReader;
	unsigned int index = 0;

	const auto basePath = "./Data/Saves/" + m_pathname + "/Team/";

	do {
		const std::string& id = ska::StringUtils::intToStr(index);
		pkmnReader.load(basePath + id + ".ini");
		if(pkmnReader.isLoaded()) {


			const auto charId = pkmnReader.get<int>("Data id");
			const auto charExp = pkmnReader.get<unsigned int>("Data experience");
			const auto charHp = pkmnReader.get<unsigned int>("Stats hp");

			const auto pokemonDBPath = "./Data/Monsters/" + ska::StringUtils::intToStr(charId) + ".ini";
			ska::IniReader detailsReader(pokemonDBPath);
			EntityLoadEvent ele(detailsReader, charId, charHp);
			ele.stats->setExperience(charExp);
			m_ged.ska::Observable<EntityLoadEvent>::notifyObservers(ele);
		}
		index++;
	} while (pkmnReader.isLoaded());

}

const std::string& SavegameManager::getStartChipsetName() {
	return m_startMapChipsetName;
}

const std::string& SavegameManager::getStartMapName() {
	return m_startMapName;
}

const std::string& SavegameManager::getPathName() {
	return m_pathname;
}


void SavegameManager::saveTrainer(const std::string& mapName, int blockX, int blockY) {
	const auto path = "./Data/Saves/" + m_pathname;
	ska::FileUtils::createDirectory(path);
	ska::FileUtils::createDirectory(path + "/Team");
	// The other keys (ex : the items) are kept
	auto reader = ska::IniReader{ path + "/trainer.ini" };
	reader.set("Trainer start_map_name", mapName);
	reader.set("Trainer start_posx", blockX);
	reader.set("Trainer start_posy", blockY);
	reader.save(path + "/trainer.ini");
	m_startMapName = mapName;
}

void SavegameManager::loadTrainer() {

	ska::IniReader reader("./Data/Saves/" + m_pathname + "/trainer.ini");

	m_startMapName = reader.get<std::string>("Trainer start_map_name");

	std::string buf = "./Levels/";
	buf += m_startMapName;
	buf +=  "/";
	buf += m_startMapName;
	buf += ".ini";

	ska::IniReader mapReader(buf);
	m_startMapChipsetName = mapReader.get<std::string>("Chipset file");

	for (unsigned int i = 0; reader.exists("Items " + ska::StringUtils::intToStr(i) + "_id"); i++) {
		const auto& id = ska::StringUtils::intToStr(i);
		loadItem(reader.get<int>("Items " + id + "_id"), reader.get<unsigned int>("Items " + id + "_amount"));
	}

}

void SavegameManager::loadItem( int, unsigned int) {
	//wScreen.getInventory().add(id, amount);
}



