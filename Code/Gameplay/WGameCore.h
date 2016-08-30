#pragma once
#include <iostream>
#include <string>
#include <memory>
#include "../ska/Graphic\GUI\Window.h"
#include "Data\Settings.h"
#include "../ska/Inputs/RawInputListener.h"
#include "../Graphic\GUI\TrainerCard.h"
#include "Data\SavegameManager.h"

#include "../ska/Utils\Singleton_template.h"
#include "../ska/Inputs/InputContextManager.h"
#include "World/WorldScene.h"
#include "CustomEntityManager.h"
#include "Scene/SceneHolderCore.h"
//#include "Inventory\Inventory.h"
#include "../ska/Utils/FpsCalculator.h"

class LayerE;
class Layer;
class DialogMenu;

typedef std::unique_ptr<DialogMenu> DialogMenuPtr;


class WGameCore : public ska::Window {

public:
	WGameCore(const std::string& title, const unsigned int w, const unsigned int h);
	virtual ~WGameCore();

	void graphicUpdate(void);
	void eventUpdate(bool movingDisallowed);

	void resize(unsigned int w, unsigned int h);
	bool refresh();
    void activeEcritureLog();
		
	void initNewWorld();
	void transition(int type);
	
	//Inventory& getInventory();
	Settings& getSettings();
	//PokemonManager& getPokemonManager();
	TrainerCard& getTrainerCard();
	ska::World& getWorld();
	WorldScene& getWorldScene();
	
	ska::ScenePtr& getScene();
	void nextScene(std::unique_ptr<ska::Scene>& scene);

protected:
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


};
