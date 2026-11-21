#pragma once
#include <memory>
#include "../Data/ItemUse.h"
#include <memory>
#include "StateFight.h"
#include "Core/State/StateBase.h"

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
	StateMap(CustomEntityManager& em, PokemonGameEventDispatcher& pged, WorldState& ws, std::string worldFileName, std::string worldChipsetName, std::optional<ska::Point<int>> trainerPosition = std::optional<ska::Point<int>>());
	
	~StateMap() override = default;

private:
	void beforeLoad(ska::State* lastState) override final;
	void afterLoad(ska::State* lastState) override final;
	bool onGameEvent(ska::GameEvent& ge);

	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;

	WorldState& m_worldState;
	const std::string m_fileName;
	std::string m_tilesetName;
	ska::ScriptRefreshSystem* m_scriptSystem = nullptr;
	// Keys using the items of the bag
	std::unique_ptr<ItemShortcuts> m_itemShortcuts;

	std::optional<ska::Point<int>> m_trainerPosition;

};

