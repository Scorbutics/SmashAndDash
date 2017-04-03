#pragma once
#include "../Graphic/GUI/TrainerCard.h"

#include "FpsCalculator.h"
#include "Data/Settings.h"

#include "Core/GameCore.h"
#include "CustomEntityManager.h"
#include "Draw/VectorDrawableContainer.h"
#include "PokemonGameEventDispatcher.h"
#include "../Audio/PokemonSoundRenderer.h"
#include "World/WorldScene.h"
#include "Scene/SceneGUIMap.h"

class WGameCore :
	public ska::GameCore<CustomEntityManager, PokemonGameEventDispatcher, ska::VectorDrawableContainer, PokemonSoundRenderer> {

public:
	WGameCore(const std::string& title, const unsigned int w, const unsigned int h);
	virtual ~WGameCore() = default;

private:
	TrainerCard m_trainerCard;
	Settings m_settings;

	std::unique_ptr<WorldScene> m_worldScene;
	std::unique_ptr<SceneGUIMap> m_guiMapScene;

	ska::FpsCalculator m_fpsCalculator;

};
