#pragma once
#include "../Graphic/GUI/TrainerCard.h"
#include "World/WorldScene.h"
#include "FpsCalculator.h"
#include "Data/Settings.h"

#include "Core/GameCore.h"
#include "CustomEntityManager.h"
#include "Draw/VectorDrawableContainer.h"
#include "PokemonGameEventDispatcherDeclaration.h"
#include "PokemonGameEventDispatcher.h"
#include "../Audio/PokemonSoundRenderer.h"

class WGameCore :
	public ska::GameCore<CustomEntityManager, PokemonGameEventDispatcher, ska::VectorDrawableContainer, PokemonSoundRenderer> {

public:
	WGameCore(const std::string& title, const unsigned int w, const unsigned int h);
	virtual ~WGameCore() = default;

	//Inventory& getInventory();
	Settings& getSettings();
	//PokemonManager& getPokemonManager();
	//TrainerCard& getTrainerCard();
	ska::World& getWorld();
	WorldScene& getWorldScene();

	ska::ScenePtr& getScene();

private:
	//PokemonManager m_pkmnManager; //Une instance de gestion des pokémon de la team
	TrainerCard m_trainerCard;
	Settings m_settings;
	//Inventory m_inv;
	WorldScene m_worldScene;
	ska::FpsCalculator m_fpsCalculator;

};
