#pragma once
#include "GUI/Windows/Window.h"
#include "Scene/Scene.h"
#include "PokemonGameEventDispatcherDeclaration.h"
#include "../Audio/PokemonSoundRenderer.h"
#include "CustomEntityManager.h"
#include "Scene/SceneHolderCore.h"
#include "../Graphic/GUI/TrainerCard.h"
#include "World/WorldScene.h"
#include "FpsCalculator.h"
#include "Data/Settings.h"

class WGameCore :
	public ska::Window {

public:
	WGameCore(const std::string& title, const unsigned int w, const unsigned int h);
	virtual ~WGameCore() = default;

	void graphicUpdate(void);
	void eventUpdate(bool movingDisallowed);

	bool refresh();

	//Inventory& getInventory();
	Settings& getSettings();
	//PokemonManager& getPokemonManager();
	//TrainerCard& getTrainerCard();
	ska::World& getWorld();
	WorldScene& getWorldScene();

	ska::ScenePtr& getScene();
	void nextScene(std::unique_ptr<ska::Scene>& scene);

private:
	PokemonGameEventDispatcher m_eventDispatcher;
	PokemonSoundRenderer m_soundManager;

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
