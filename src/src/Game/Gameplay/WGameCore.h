#pragma once
#include "../Graphic/GUI/TrainerCard.h"

#include "Data/Settings.h"

#include "Core/GameCore.h"
#include "CustomEntityManager.h"
#include "PokemonGameEventDispatcher.h"
#include "../Audio/PokemonSoundRenderer.h"
#include "World/WorldState.h"
#include "State/StateGUIMap.h"
#include "State/StateMap.h"
#include "CoreModule.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Data/Events/StateEvent.h"
#include "Core/Data/Events/GameEvent.h"
#include "GraphicModule.h"
#include "SoundModule.h"

using GameBase = ska::GameCore<PokemonGameEventDispatcher, ska::CoreModule<CustomEntityManager>, ska::GraphicModule, ska::SoundModule<PokemonSoundRenderer>>;

namespace ska {
	class StateBase;
}

class WGameCore :
	public GameBase,
	public ska::SubObserver<MapEvent>,
	public ska::SubObserver<ska::StateEvent>,
	public ska::SubObserver<ska::GameEvent> {

public:
	using GameConfPtr = GameConfPtr;
	using GameConf = GameConf;

	WGameCore(CustomEntityManager& em, GameConfPtr&& gc);

	int onTerminate(ska::TerminateProcessException& te) override;
	int onException(ska::GenericException& ge) override;

	float ticksWanted() const override;

	virtual ~WGameCore() = default;

private:
	// Plays a save : the world, its start map, the interface
	void startWorld(const std::string& save);
	bool onStateEvent(ska::StateEvent& se);
	bool onGameEvent(ska::GameEvent& ge);
	bool onTeleport(MapEvent& me);
	void switchState(const MapEvent& me);

	CustomEntityManager& m_entityManager;

	TrainerCard m_trainerCard;
	Settings m_settings;

	WorldState* m_worldState = nullptr;
	// The title screen was the first state : the world started after it is told the window is ready
	bool m_titleShown = false;
	bool m_worldPending = false;
	ska::Point<unsigned int> m_windowSize{ 0, 0 };
	std::unique_ptr<StateGUIMap> m_guiMapScene;

	ska::StateBase* m_currentState = nullptr;
	std::unique_ptr<ska::StateBase> m_nextState;

};
