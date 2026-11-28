#include "Game/__internalConfig/LoggerConfig.h"
#include "Game/Gameplay/Data/SavePaths.h"
#include <chrono>
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
#include "Game/Gameplay/World/Generation/InfiniteFiles.h"
#include "Game/Gameplay/World/Chunks/ChunkGenerator.h"
#include "SaveSlots.h"

SavegameManager::SavegameManager(ska::EntityManager& em, PokemonGameEventDispatcher& ged, const std::string& filename):
	m_entityManager(em),
	m_ged(ged) {

	m_pathname = filename;
	loadTrainer();
}


void SavegameManager::newGame() {
	m_pathname = "new_game.ini";
}


std::vector<std::pair<std::string, int>> SavegameManager::getGameVariables() const {
	auto variables = std::vector<std::pair<std::string, int>>{ m_game_variables.begin(), m_game_variables.end() };
	std::sort(variables.begin(), variables.end());
	return variables;
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

	std::ofstream tmpScriptList((SaveDirectory() + m_pathname + "/tmpscripts.data").c_str(), std::ios::trunc);
}

// variables.ini : the [Variable] section, "name=value" (the numbered variables of the old scripts are named "%00001"...)
void SavegameManager::loadVariables() {
	m_game_variables.clear();
	auto file = std::ifstream{ SaveDirectory() + m_pathname + "/variables.ini" };
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
	auto file = std::ofstream{ SaveDirectory() + m_pathname + "/variables.ini", std::ios::trunc };
	file << "[Game]\nvar_number=" << names.size() << "\n[Variable]\n";
	for (const auto& name : names) {
		file << name << "=" << m_game_variables.at(name) << "\n";
	}
}

void SavegameManager::loadPokemonTeam() {
	ska::IniReader pkmnReader;
	unsigned int index = 0;

	const auto basePath = SaveDirectory() + m_pathname + "/Team/";

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
	const auto path = SaveDirectory() + m_pathname;
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

// A new game : a save that does not exist yet is made from Data/Saves/new_game, its world to generate (SaveSlots). A
// world to generate is generated in the save at once (the title screen generates it step by step before : StateTitle).
void SavegameManager::prepareSave() {
	if (!SaveSlots::Exists(m_pathname)) {
		SaveSlots::Create(m_pathname, SaveSlots::NewSeed());
	}
	const auto seed = SaveSlots::WorldToGenerate(m_pathname);
	if (!seed) {
		return;
	}
	const auto begin = std::chrono::steady_clock::now();
	const auto start = worldgen::GenerateInfiniteWorld(*seed, SaveSlots::WorldPath(m_pathname), SaveSlots::WorldPath(m_pathname));
	SLOG(ska::LogLevel::Info) << "Savegame : the world of seed " << *seed << " generated in "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() << " ms (" << SaveSlots::WorldPath(m_pathname) << ")";
	SaveSlots::WorldGenerated(m_pathname, *seed, start);
}

void SavegameManager::loadTrainer() {
	prepareSave();
	ska::IniReader reader(SaveDirectory() + m_pathname + "/trainer.ini");

	m_startMapName = reader.get<std::string>("Trainer start_map_name");
	// The world of the save : its own folder of maps, or the maps of Levels
	auto world = reader.exists("World directory") ? reader.get<std::string>("World directory") : std::string{ "./Levels/" };
	if (!world.empty() && world.back() != '/') {
		world += '/';
	}
	WorldDirectoryRef() = world;
	// An infinite world : the chunk of the trainer written (the game reads its map now ; a world written again keeps the
	// trainer where it was)
	if (const auto seed = worldgen::InfiniteSeed(world)) {
		const auto chunk = chunks::Parse(m_startMapName);
		auto writer = chunks::TakeOver(world);
		if (writer == nullptr) {
			writer = std::make_unique<worldgen::InfiniteWriter>(*seed, world, world);
		}
		if (chunk && !writer->written(chunk->x, chunk->y)) {
			writer->write(chunk->x, chunk->y);
		}
		chunks::HandOver(std::move(writer));
	}

	ska::IniReader mapReader(MapIni(m_startMapName));
	m_startMapChipsetName = mapReader.get<std::string>("Chipset file");

	for (unsigned int i = 0; reader.exists("Items " + ska::StringUtils::intToStr(i) + "_id"); i++) {
		const auto& id = ska::StringUtils::intToStr(i);
		loadItem(reader.get<int>("Items " + id + "_id"), reader.get<unsigned int>("Items " + id + "_amount"));
	}

}

void SavegameManager::loadItem( int, unsigned int) {
	//wScreen.getInventory().add(id, amount);
}



