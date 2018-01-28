#pragma once
#include "../Graphic/GUI/TrainerCard.h"

#include "Data/Settings.h"

#include "Core/GameCore.h"
#include "CustomEntityManager.h"
#include "PokemonGameEventDispatcher.h"
#include "../Audio/PokemonSoundRenderer.h"
#include "World/WorldState.h"
#include "State/StateGUIMap.h"
#include "FpsCalculator.h"
#include "State/StateMap.h"
#include "CoreModule.h"
#include "GraphicModule.h"
#include "SoundModule.h"

using GameBase = ska::GameCore<PokemonGameEventDispatcher, ska::CoreModule<CustomEntityManager>, ska::GraphicModule, ska::SoundModule<PokemonSoundRenderer>>;

class WGameCore :
	public GameBase,
	public ska::SubObserver<MapEvent> {

public:
	using GameConfPtr = GameConfPtr;
	using GameConf = GameConf;

	WGameCore(CustomEntityManager& em, GameConfPtr&& gc);

	virtual int onTerminate(ska::TerminateProcessException& te) override;
	virtual int onException(ska::GenericException& ge) override;

	virtual float ticksWanted() const override;

	virtual ~WGameCore() = default;

private:
	bool onTeleport(MapEvent& me);

	CustomEntityManager& m_entityManager;

	TrainerCard m_trainerCard;
	Settings m_settings;

	WorldState* m_worldState;
	std::unique_ptr<StateGUIMap> m_guiMapScene;

	ska::FpsCalculator m_fpsCalculator;
	AbstractStateMap* m_currentState;
	std::unique_ptr<ska::StateBase> m_nextState;

};
