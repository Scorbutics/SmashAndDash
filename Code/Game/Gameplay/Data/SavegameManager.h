#pragma once

#include <vector>
#include "../PokemonGameEventDispatcher.h"
#include "Data/Savegame.h"
#include "../PokemonGameEventDispatcherDeclaration.h"

class SavegameManager : public ska::Savegame {
public:
	explicit SavegameManager(PokemonGameEventDispatcher& ged, const std::string& filename);
	explicit SavegameManager(const std::string& pathname);

	SavegameManager& operator=(const SavegameManager&) = delete;

	void newGame();
	void loadGame(const std::string& filename);
	void saveGame(const std::string& filename);

	virtual std::string getSaveName() const override;
	virtual int getGameVariable(const unsigned int x) const override;
	virtual bool getGameSwitch(const unsigned int x) const override;

	virtual void setGameVariable(const unsigned int x, const int value) override;
	virtual void setGameSwitch(const unsigned int x, const bool value) override;

	const std::string& getStartChipsetName();
	const std::string& getPathName();
	const std::string& getStartMapName();
	~SavegameManager();

private:
	void loadItem(int id, unsigned int amount);
	void loadTrainer();
	void saveTrainer();
	void saveItems();
	void savePokemonTeam();
	void loadPokemonTeam();

	PokemonGameEventDispatcher& m_ged;

	std::string m_pathname, m_startMapChipsetName, m_startMapName;
	std::vector<int> m_game_variables;
	std::vector<bool> m_game_switches;

};

