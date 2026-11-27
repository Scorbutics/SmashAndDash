#pragma once
#include <memory>
#include "../Data/ItemUse.h"
#include "../Control/InputHooks.h"
#include <memory>
#include "StateFight.h"
#include "Core/State/StateBase.h"
#include "../../Graphic/GUI/WildPokemonMarks.h"
#include "../Control/WalkFeelSystem.h"

struct WorldStateChanger;
class WorldState;

namespace ska {
	class Window;
	class ScriptRefreshSystem;
	class CameraSystem;
}

class StateMap :
	public ska::StateBase,
	public ska::SubObserver<ska::GameEvent> {
public:
	StateMap(CustomEntityManager& em, PokemonGameEventDispatcher& pged, WorldState& ws, std::string worldFileName, std::string worldChipsetName, std::optional<ska::Point<int>> trainerPosition = std::optional<ska::Point<int>>(), bool walkIn = false, bool resume = false);
	
	~StateMap() override = default;

private:
	void walkIn(ska::EntityId trainer);

public:

private:
	void beforeLoad(ska::State* lastState) override final;
	void afterLoad(ska::State* lastState) override final;
	void onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;
	bool onGameEvent(ska::GameEvent& ge);

	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;

	WorldState& m_worldState;
	const std::string m_fileName;
	std::string m_tilesetName;
	ska::ScriptRefreshSystem* m_scriptSystem = nullptr;
	// Keys using the items of the bag
	std::unique_ptr<ItemShortcuts> m_itemShortcuts;
	// Engine events of the keys for the scripts
	std::unique_ptr<InputHooks> m_inputHooks;

	std::optional<ska::Point<int>> m_trainerPosition;
	// The player comes through a door : it walks from the block behind its position (MapEvent::walkIn)
	bool m_walkIn = false;
	// Back from a battle : the map is still there, it is not loaded again (MapEvent::resume)
	bool m_resume = false;
	// The marks over the wild Pokemon that notice the player
	std::unique_ptr<WildPokemonMarks> m_wildMarks;
	// Owned by the logic of the state
	WalkFeelSystem* m_walkFeel = nullptr;

};

