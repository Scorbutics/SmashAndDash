#pragma once
#include <iostream>
#include <string>
#include <memory>
#include "../ska/Graphic/GUI/Window.h"
#include "Data/Settings.h"
#include "../ska/Inputs/RawInputListener.h"
#include "../Graphic/GUI/TrainerCard.h"
#include "Data/SavegameManager.h"

#include "../ska/Core/EventDispatcher.h"
#include "../ska/Utils/Singleton_template.h"
#include "../ska/Inputs/InputContextManager.h"
#include "World/WorldScene.h"
#include "CustomEntityManager.h"
#include "Scene/SceneHolderCore.h"
#include "../ska/Core/GameEventDispatcher.h"
//#include "Inventory\Inventory.h"
#include "../ska/Utils/FpsCalculator.h"
#include "../ska/Core/Game.h"

class WGameCore : 
	public ska::Window, 
	public ska::Game {

public:
	WGameCore(const std::string& title, const unsigned int w, const unsigned int h);
	virtual ~WGameCore() = default;

	void graphicUpdate(void);
	void eventUpdate(bool movingDisallowed);

	bool refresh();
    void activeEcritureLog();

	void initNewWorld();
	
	//Inventory& getInventory();
	Settings& getSettings();
	//PokemonManager& getPokemonManager();
	TrainerCard& getTrainerCard();
	ska::World& getWorld();
	WorldScene& getWorldScene();

	ska::ScenePtr& getScene();
	void nextScene(std::unique_ptr<ska::Scene>& scene);

private:
	
	ska::GameEventDispatcher m_eventDispatcher;
	ska::RawInputListener m_rawInputListener;
	ska::InputContextManager m_playerICM;

	CustomEntityManager m_entityManager;

	SceneHolderCore m_sceneHolder;

	//PokemonManager m_pkmnManager; //Une instance de gestion des pokémon de la team
	TrainerCard m_trainerCard;
	Settings m_settings;
	//Inventory m_inv;
	WorldScene m_worldScene;
	ska::FpsCalculator m_fpsCalculator;
	
	bool refreshInternal();


};
