#pragma once
#include "../Graphic/GUI/TrainerCard.h"
#include "World/WorldScene.h"
#include "FpsCalculator.h"
#include "Data/Settings.h"

#include "Core/GameCore.h"
#include "CustomEntityManager.h"
#include "Draw/VectorDrawableContainer.h"
#include "PokemonGameEventDispatcher.h"
#include "../Audio/PokemonSoundRenderer.h"

class WGameCore :
	public ska::GameCore<CustomEntityManager, PokemonGameEventDispatcher, ska::VectorDrawableContainer, PokemonSoundRenderer> {

public:
	WGameCore(const std::string& title, const unsigned int w, const unsigned int h);
	virtual ~WGameCore() = default;

	//Inventory& getInventory();
	//PokemonManager& getPokemonManager();
	//TrainerCard& getTrainerCard();

private:
	//PokemonManager m_pkmnManager; //Une instance de gestion des pokémon de la team
	TrainerCard m_trainerCard;
	Settings m_settings;
	std::unique_ptr<WorldScene> m_worldScene;
	//Inventory m_inv;
	ska::FpsCalculator m_fpsCalculator;

};
