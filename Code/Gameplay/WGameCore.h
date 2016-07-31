#pragma once
#include <iostream>
#include <string>
#include <memory>
#include "../ska/Graphic\GUI\Window.h"
#include "Data\Settings.h"
#include "../ska/Inputs/RawInputListener.h"
#include "../Graphic\GUI\TrainerCard.h"
#include "Data\SavegameManager.h"

#include "../Graphic/GUI/GUI.h"
#include "../ska/Utils\Singleton_template.h"
#include "../ska/Inputs/InputContextManager.h"
#include "World/WorldScene.h"
#include "CustomEntityManager.h"
#include "Scene/SceneHolderCore.h"
//#include "Inventory\Inventory.h"
#include "../ska/Task/TaskQueue.h"
#include "../ska/Utils/FpsCalculator.h"

class LayerE;
class Layer;
class DialogMenu;

typedef std::unique_ptr<DialogMenu> DialogMenuPtr;


class WGameCore : public ska::Window, public ska::Singleton<WGameCore> {
	friend class ska::Singleton<WGameCore>;

private:
    WGameCore();
	~WGameCore();

public:
	void graphicUpdate(void);
	void eventUpdate(bool movingDisallowed);

	void resize(unsigned int w, unsigned int h);
	bool refresh();
    void activeEcritureLog();
		
	void initNewWorld();
	void transition(int type);
	
	
	void addTaskToQueue(ska::RunnablePtr& t);
	bool hasRunningTask();

	GUI& getGUI();

	//Inventory& getInventory();
	Settings& getSettings();
	//PokemonManager& getPokemonManager();
	TrainerCard& getTrainerCard();
	ska::World& getWorld();
	WorldScene& getWorldScene();
	
	const ska::InputActionContainer& getActions() const;
	const ska::InputRangeContainer& getRanges() const;
	const ska::InputToggleContainer& getToggles() const;
	
	ska::ScenePtr& getScene();
	void nextScene(std::unique_ptr<ska::Scene>& scene);

protected:
	ska::RawInputListener m_rawInputListener;
	ska::InputContextManager m_playerICM;
		
	CustomEntityManager m_entityManager;
		
	SceneHolderCore m_sceneHolder;

	//PokemonManager m_pkmnManager; //Une instance de gestion des pok�mon de la team
	TrainerCard m_trainerCard;
	GUI m_gui;
	Settings m_settings;
	//Inventory m_inv;
	WorldScene m_worldScene;
	ska::FpsCalculator m_fpsCalculator;

	ska::TaskQueue m_taskQueue;


};
