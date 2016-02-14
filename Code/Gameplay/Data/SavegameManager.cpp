#include <iostream>
#include <fstream>
#include "SavegameManager.h"
#include "Utils\IDs.h"
#include "Utils\ChargementImages.h"
#include "..\WGameCore.h"
#include "Utils\StringUtils.h"
#include "..\Character.h"
#include "Gameplay\Data\Statistics.h"

using namespace std;

SavegameManager::SavegameManager(std::string pathname)
{
	m_pathname = pathname;
}

void SavegameManager::newGame()
{
	m_pathname = "new_game.ini";
}


int SavegameManager::getGameVariable(unsigned int x)
{
	if(x < m_game_variables.size())
		return m_game_variables[x];
	else
	{
		cerr << "Erreur (classe WGameCore) : Buffer Overflow lors de la tentative d'accès à la game variable numéro " << x << endl;
		return 0;
	}
}

bool SavegameManager::getGameSwitch(unsigned int x)
{
	if(x < m_game_switches.size())
		return m_game_switches[x];
	else
	{
		cerr << "Erreur (classe WGameCore) : Buffer Overflow lors de la tentative d'accès au game switch numéro " << x << endl;
		return 0;
	}
}

void SavegameManager::setGameVariable(unsigned int x, int value)
{
	if(x < m_game_variables.size())
		m_game_variables[x] = value;

}

void SavegameManager::setGameSwitch(unsigned int x, bool value)
{
	if(x < m_game_switches.size())
		m_game_switches[x] = value;

}

void SavegameManager::saveGame( std::string pathname )
{
	m_pathname = pathname;
	saveTrainer();
	savePokemonTeam();

	SwitchesWriting(m_game_switches, "."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR +m_pathname + FILE_SEPARATOR"switches.ini");
	VariablesWriting(m_game_variables, "."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"variables.ini");
}

std::string SavegameManager::getSaveName()
{
	return m_pathname;
}

void SavegameManager::loadGame( std::string pathname )
{
	m_pathname = pathname;
	loadTrainer();
	loadPokemonTeam();

	SwitchesAcquisition(m_game_switches, "."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR +m_pathname + FILE_SEPARATOR"switches.ini");
	VariablesAcquisition(m_game_variables, "."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"variables.ini");

	ofstream tmpScriptList(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"tmpscripts.data").c_str(), ios::trunc);
}

void SavegameManager::savePokemonTeam()
{
	IniReader reader;
	WGameCore& wScreen = WGameCore::getInstance();

	for (unsigned int i = 0; i < 6; i++) {
		const std::string& id = StringUtils::intToStr(i);
		remove(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"Team" + FILE_SEPARATOR + id + ".ini").c_str());
	}
		

	for(unsigned int i = 0; i < wScreen.getPokemonManager().getPokemonTeamSize(); i++)
	{
		const std::string& id = StringUtils::intToStr(i);
		ofstream of("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"Team" + FILE_SEPARATOR + id + ".ini");
		of.close();
		reader.load("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"Team" + FILE_SEPARATOR + id + ".ini");
		if(reader.isLoaded())
		{
			Character* pkmn = wScreen.getPokemonManager().getPokemon(i);
			reader.set("Data id", pkmn->getID());
			reader.set("Data experience", pkmn->getStatistics()->getExperience());
			reader.set("Stats hp", pkmn->getHp());
			reader.save("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"Team" + FILE_SEPARATOR + id + ".ini");
		}
	}
}

void SavegameManager::loadPokemonTeam()
{
	IniReader reader;
	unsigned int index = 0;
	WGameCore& wScreen = WGameCore::getInstance();

	do 
	{
		const std::string& id = StringUtils::intToStr(index);
		reader.load("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"Team" + FILE_SEPARATOR + id + ".ini");
		if(reader.isLoaded())
		{
			wScreen.getPokemonManager().add(reader.getInt("Data id"));
			Character* pkmn = wScreen.getPokemonManager().getLastPokemon();
			pkmn->getStatistics()->setExperience(reader.getInt("Data experience"));
			pkmn->setHP(reader.getInt("Stats hp"));
		}
		index++;
	} while (reader.isLoaded());
	
}

string SavegameManager::getStartChipsetName()
{
	return m_startMapChipsetName;
}

string SavegameManager::getStartMapName()
{
	return m_startMapName;
}

void SavegameManager::saveTrainer()
{
	Character* hero;
	ofstream of("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"trainer.ini");
	WGameCore& wScreen = WGameCore::getInstance();

	if(of.fail())
	{
		CreateDirectory(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname).c_str(),0);
		CreateDirectory(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"Team").c_str(),0);
		of.open("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"trainer.ini");
	}
	of.close();
	IniReader reader("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"trainer.ini");

	hero = wScreen.getEntityFactory().getTrainer();
	reader.set("Trainer start_posx", hero->getHitboxCenterPos().x/TAILLEBLOC);
	reader.set("Trainer start_posy", hero->getHitboxCenterPos().y/TAILLEBLOC);
	reader.set("Trainer start_map_name", wScreen.getWorld().getName());
	reader.save("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"trainer.ini");

	saveItems();
}

void SavegameManager::saveItems()
{
	IniReader reader("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"trainer.ini");
	WGameCore& wScreen = WGameCore::getInstance();
	for(unsigned int i = 0; i < wScreen.getInventory().getSize(); i++)
	{
		const std::string& id = StringUtils::intToStr(i);
		reader.set("Items " + id + "_id", wScreen.getInventory().getObjectFromIndex(i)->getID());
		reader.set("Items " + id + "_amount", wScreen.getInventory().getAmountFromIndex(i));
	}
	reader.save("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"trainer.ini");
}

void SavegameManager::loadTrainer()
{
	Character* hero;
	unsigned int startPosx, startPosy;
	IniReader reader("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + m_pathname + FILE_SEPARATOR"trainer.ini");
	WGameCore& wScreen = WGameCore::getInstance();

	startPosx = reader.getInt("Trainer start_posx");
	startPosy = reader.getInt("Trainer start_posy");
	m_startMapName = reader.getString("Trainer start_map_name");

	string buf = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR;
	buf += m_startMapName;
	buf +=  FILE_SEPARATOR;
	buf += m_startMapName;
	buf += ".ini";

	IniReader mapReader(buf);
	m_startMapChipsetName = mapReader.getString("Chipset file");

	if(m_startMapChipsetName == "STRINGNOTFOUND")
		cerr << "Erreur : impossible de trouver le nom du chipset de la map de depart" << endl;

	hero = wScreen.getEntityFactory().getTrainer();
	hero->teleport(startPosx*TAILLEBLOC, startPosy*TAILLEBLOC);
	
	for (unsigned int i = 0; reader.get("Items " + StringUtils::intToStr(i) + "_id"); i++) {
		const std::string& id = StringUtils::intToStr(i);
		loadItem(reader.getInt("Items " + id + "_id"), (unsigned int)reader.getInt("Items " + id + "_amount"));
	}
		
}

void SavegameManager::loadItem( int id, unsigned int amount )
{
	WGameCore& wScreen = WGameCore::getInstance();
	wScreen.getInventory().add(id, amount);
}

SavegameManager::~SavegameManager()
{

}


