#ifndef DEF_SAVEGAMEMANAGER
#define DEF_SAVEGAMEMANAGER

#include <vector>
#include <string>
#include <iostream>
#include "./ska/Savegame.h"

class SavegameManager : public ska::Savegame
{
	public:
		SavegameManager(std::string filename);
		void newGame();
		void loadGame(std::string filename);
		void saveGame(std::string filename);
		
		virtual std::string getSaveName() const override;
		virtual int getGameVariable(const unsigned int x) const override;
		virtual bool getGameSwitch(const unsigned int x) const override;

		virtual void setGameVariable(const unsigned int x, const int value) override;
		virtual void setGameSwitch(const unsigned int x, const bool value) override;

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
