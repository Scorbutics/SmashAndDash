#ifndef DEF_SAVEGAMEMANAGER
#define DEF_SAVEGAMEMANAGER

#include <vector>
#include <string>
#include <iostream>

class SavegameManager
{
	public:
		SavegameManager(std::string filename);
		void newGame();
		void loadGame(std::string filename);
		void saveGame(std::string filename);
		
		std::string getSaveName();
		int getGameVariable(unsigned int x);
		bool getGameSwitch(unsigned int x);
		void setGameVariable(unsigned int x, int value);
		void setGameSwitch(unsigned int x, bool value);
		std::string getStartChipsetName();
		std::string getStartMapName();
		~SavegameManager();

	private:
		void loadItem(int id, unsigned int amount);
		void loadTrainer();
		void saveTrainer();
		void saveItems();
		void savePokemonTeam();
		void loadPokemonTeam();
		std::string m_pathname, m_startMapChipsetName, m_startMapName;
		std::vector<int> m_game_variables;
		std::vector<bool> m_game_switches;

};

#endif
