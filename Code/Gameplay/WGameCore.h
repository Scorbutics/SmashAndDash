#pragma once

#include <windows.h>

#include <iostream>
#include <string>
#include <memory>
#include "../ska/Graphic\GUI\Window.h"
#include "Pokeball.h"
#include "Data\Settings.h"
#include "../Graphic\GUI\MouseCursor.h"
#include "../ska/Graphic\Rectangle.h"
#include "../ska/Inputs/RawInputListener.h"
#include "../Graphic\GUI\TrainerCard.h"
#include "Data\SavegameManager.h"

#include "../Graphic/GUI/GUI.h"
#include "../ska/Utils\Singleton_template.h"
#include "../ska/Inputs/InputContextManager.h"
#include "World/WorldScene.h"
#include "CustomEntityManager.h"
#include "Scene/SceneHolderCore.h"
#include "Inventory\Inventory.h"
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
	void waitQuit(DialogMenu* window);
	void addTaskToQueue(ska::RunnablePtr& t);
	bool hasRunningTask();

	float getSpeedInertie();
	ska::Animation& getChipsetAnimation();

	GUI& getGUI();
	
	Pokeball& getPokeball();
	Inventory& getInventory();
	Settings& getSettings();
	//PokemonManager& getPokemonManager();
	MouseCursor& getMouseCursor();
	TrainerCard& getTrainerCard();
	ska::World& getWorld();
	WorldScene& getWorldScene();
	
	const ska::InputActionContainer& getActions() const;
	const ska::InputRangeContainer& getRanges() const;
	const ska::InputToggleContainer& getToggles() const;
	
	void setOffsetChipset(int x, int y, int w, int h);
    void setSpeedInertie(float x);
    void setChipset(SDL_Surface* chipset);
	
	ska::ScenePtr& getScene();
	void nextScene(std::unique_ptr<ska::Scene>& scene);

protected:
	ska::RawInputListener m_rawInputListener;
	ska::InputContextManager m_playerICM;
		
	CustomEntityManager m_entityManager;
		
	SceneHolderCore m_sceneHolder;

	ska::Rectangle m_OfChip;
	bool m_ecritureLog;

	float m_speedInertie;
	ska::Animation m_chipsetAni;
	//AI m_ai;
	Pokeball m_pokeball;
	//PokemonManager m_pkmnManager; //Une instance de gestion des pokémon de la team
	TrainerCard m_trainerCard;
	MouseCursor m_mouseCursor;
	GUI m_gui;
	Settings m_settings;
	Inventory m_inv;
	WorldScene m_worldScene;
	ska::FpsCalculator m_fpsCalculator;

	ska::TaskQueue m_taskQueue;


};
