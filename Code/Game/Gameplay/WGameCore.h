#pragma once
#include "../Graphic/GUI/TrainerCard.h"

#include "Data/Settings.h"

#include "Core/GameCore.h"
#include "CustomEntityManager.h"
#include "Draw/VectorDrawableContainer.h"
#include "PokemonGameEventDispatcher.h"
#include "../Audio/PokemonSoundRenderer.h"
#include "World/WorldState.h"
#include "State/StateGUIMap.h"
#include "FpsCalculator.h"

class WGameCore :
	public ska::GameCore<CustomEntityManager, PokemonGameEventDispatcher, ska::VectorDrawableContainer, PokemonSoundRenderer> {

public:
	
	WGameCore(RendererPtr&& renderer, WindowPtr&& window);

	virtual int onTerminate(ska::TerminateProcessException& te) override;
	virtual int onException(ska::GenericException& ge) override;

	virtual float ticksWanted() const override;

	virtual ~WGameCore() = default;

private:
	TrainerCard m_trainerCard;
	Settings m_settings;

	std::unique_ptr<WorldState> m_worldScene;
	std::unique_ptr<StateGUIMap> m_guiMapScene;

	ska::FpsCalculator m_fpsCalculator;

};
