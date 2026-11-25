#pragma once
#include "Core/ECS/EntityManager.h"
#include "../PokemonGameEventDispatcher.h"
#include "Core/Data/MemoryScript.h"

class SavegameManager : 
	public ska::MemoryScript {
public:
	explicit SavegameManager(ska::EntityManager& em, PokemonGameEventDispatcher& ged, const std::string& filename = "");

	SavegameManager& operator=(const SavegameManager&) = delete;

	void newGame();
	void loadGame(const std::string& filename);
	// Saves the game : the trainer (map, block position), the game variables. The team is saved as it changes (Team).
	void saveGame(const std::string& mapName, int blockX, int blockY);

	std::string getSaveName() const override;
	int getGameVariable(const std::string& x) const override;
	std::vector<std::pair<std::string, int>> getGameVariables() const override;
	
	void setGameVariable(const std::string& x, int value) override;

	const std::string& getStartChipsetName();
	const std::string& getPathName();
	const std::string& getStartMapName();
	~SavegameManager() = default;

private:
	void loadItem(int id, unsigned int amount);
	void prepareSave();
	void loadTrainer();
	void saveTrainer(const std::string& mapName, int blockX, int blockY);
	void loadPokemonTeam();
	void loadVariables();
	void saveVariables() const;

	ska::EntityManager& m_entityManager;
	PokemonGameEventDispatcher& m_ged;

	std::string m_pathname, m_startMapChipsetName, m_startMapName;
	std::unordered_map<std::string, int> m_game_variables;

};

