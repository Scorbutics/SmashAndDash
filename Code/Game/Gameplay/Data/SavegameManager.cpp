#include <iostream>
#include <fstream>
#include "SavegameManager.h"
#include "../../Utils/ChargementImages.h"
#include "Utils/StringUtils.h"
#include "Utils/FileUtils.h"
#include "../../Gameplay/Data/Statistics.h"
#include "Logging/Logger.h"

SavegameManager::SavegameManager(PokemonGameEventDispatcher& ged, const std::string& filename):
	m_ged(ged) {
	m_pathname = filename;
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

bool SavegameManager::getGameSwitch(const std::string& x) const {
	const auto search = m_game_switches.find(x);
	if (search != m_game_switches.end()) {
		return search->second;
	}
	return false;
}

void SavegameManager::setGameVariable(const std::string& x, const int value) {
		m_game_variables[x] = value;
}

void SavegameManager::setGameSwitch(const std::string& x, const bool value) {
		m_game_switches[x] = value;
}

void SavegameManager::saveGame(const std::string& pathname) {
	m_pathname = pathname;
	saveTrainer();
	savePokemonTeam();

	SwitchesWriting(m_game_switches, "." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "switches.ini");
	VariablesWriting(m_game_variables, "." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "variables.ini");
}

std::string SavegameManager::getSaveName() const {
	return m_pathname;
}

void SavegameManager::loadGame(const std::string& pathname) {
	m_pathname = pathname;
	loadTrainer();
	loadPokemonTeam();

	SwitchesAcquisition(m_game_switches, "." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "switches.ini");
	VariablesAcquisition(m_game_variables, "." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "variables.ini");

	std::ofstream tmpScriptList(("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "tmpscripts.data").c_str(), std::ios::trunc);
}

void SavegameManager::savePokemonTeam() {
	ska::IniReader reader;

	for (unsigned int i = 0; i < 6; i++) {
		const std::string& id = ska::StringUtils::intToStr(i);
		ska::FileUtils::removeFile(("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "Team" + FILE_SEPARATOR + id + ".ini").c_str());
	}


	/*for(unsigned int i = 0; i < wScreen.getPokemonManager().getPokemonTeamSize(); i++)
	{
		const std::string& id = ska::StringUtils::intToStr(i);
		ofstream of("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "Team" + FILE_SEPARATOR + id + ".ini");
		of.close();
		reader.load("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "Team" + FILE_SEPARATOR + id + ".ini");
		if(reader.isLoaded())
		{
			Character* pkmn = wScreen.getPokemonManager().getPokemon(i);
			reader.set("Data id", pkmn->getID());
			reader.set("Data experience", pkmn->getStatistics()->getExperience());
			reader.set("Stats hp", pkmn->getHp());
			reader.save("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "Team" + FILE_SEPARATOR + id + ".ini");
		}
	}*/
}

void SavegameManager::loadPokemonTeam() {
	ska::IniReader pkmnReader;
	unsigned int index = 0;

	const auto basePath = "." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "Team" + FILE_SEPARATOR;

	do {
		const std::string& id = ska::StringUtils::intToStr(index);
		pkmnReader.load(basePath + id + ".ini");
		if(pkmnReader.isLoaded()) {


			const auto charId = pkmnReader.get<int>("Data id");
			const auto charExp = pkmnReader.get<unsigned int>("Data experience");
			const auto charHp = pkmnReader.get<unsigned int>("Stats hp");

			const auto pokemonDBPath = "." FILE_SEPARATOR "Data" FILE_SEPARATOR "Monsters" FILE_SEPARATOR + ska::StringUtils::intToStr(charId) + ".ini";
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

void SavegameManager::saveTrainer() {
	//Character* hero;
	std::ofstream of("." FILE_SEPARATOR " Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "trainer.ini");

	if(of.fail()) {
		ska::FileUtils::createDirectory("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname);
		ska::FileUtils::createDirectory("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "Team");

		of.open("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "trainer.ini");
	}

	/*ska::IniReader reader("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "trainer.ini");

	hero = wScreen.getEntityFactory().getTrainer();
	reader.set("Trainer start_posx", hero->getHitboxCenterPos().x/TAILLEBLOC);
	reader.set("Trainer start_posy", hero->getHitboxCenterPos().y/TAILLEBLOC);
	reader.set("Trainer start_map_name", wScreen.getWorld().getName());
	reader.save("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "trainer.ini");*/

	saveItems();
}

void SavegameManager::saveItems() {
	ska::IniReader reader("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "trainer.ini");
	/*for(unsigned int i = 0; i < wScreen.getInventory().getSize(); i++)
	{
		const std::string& id = ska::StringUtils::intToStr(i);
		reader.set("Items " + id + "_id", wScreen.getInventory().getObjectFromIndex(i)->getID());
		reader.set("Items " + id + "_amount", wScreen.getInventory().getAmountFromIndex(i));
	}*/
	reader.save("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "trainer.ini");
}

void SavegameManager::loadTrainer() {

	ska::IniReader reader("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + m_pathname + FILE_SEPARATOR "trainer.ini");

	/*
	TODO
	startPosx = reader.get<int>("Trainer start_posx");
	startPosy = reader.get<int>("Trainer start_posy");*/
	m_startMapName = reader.get<std::string>("Trainer start_map_name");

	std::string buf = "." FILE_SEPARATOR "Levels" FILE_SEPARATOR;
	buf += m_startMapName;
	buf +=  FILE_SEPARATOR;
	buf += m_startMapName;
	buf += ".ini";

	ska::IniReader mapReader(buf);
	m_startMapChipsetName = mapReader.get<std::string>("Chipset file");

	if(m_startMapChipsetName == "STRINGNOTFOUND")
		SKA_LOG_ERROR("Erreur : impossible de trouver le nom du chipset de la map de depart");

	/*hero = wScreen.getEntityFactory().getTrainer();
	hero->teleport(startPosx*TAILLEBLOC, startPosy*TAILLEBLOC);*/

	for (unsigned int i = 0; reader.exists("Items " + ska::StringUtils::intToStr(i) + "_id"); i++) {
		const auto& id = ska::StringUtils::intToStr(i);
		loadItem(reader.get<int>("Items " + id + "_id"), reader.get<unsigned int>("Items " + id + "_amount"));
	}

}

void SavegameManager::loadItem( int, unsigned int) {
	//wScreen.getInventory().add(id, amount);
}

SavegameManager::~SavegameManager() {

}


