#pragma once
#include "Script/System/ScriptBasicCommandsSystem.h"
//#include "Core/State/SceneSwitcher.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"

class WorldState;

namespace ska {
	class TileWorld;
}

class ScriptCommandsSystem :
	public ska::ScriptBasicCommandsSystem
{
public:
	ScriptCommandsSystem(ska::EntityManager& entityManager, ska::TileWorld& w, ska::MemoryScript& saveGame, PokemonGameEventDispatcher& ged);
	~ScriptCommandsSystem() override = default;

protected:
	struct ScriptCommandHelper : public BasicScriptCommandHelper {
		ScriptCommandHelper(ska::TileWorld& w, ska::EntityManager& e, PokemonGameEventDispatcher& ged) :
		    BasicScriptCommandHelper(w, e), m_ged(ged),
			m_world(w) {
		}

		void setupCommands(std::unordered_map<std::string, ska::CommandPtr>& c, ska::EntityLocator& locator) const override;
	private:
		ska::TileWorld& m_world;
		PokemonGameEventDispatcher& m_ged;
	};

};

