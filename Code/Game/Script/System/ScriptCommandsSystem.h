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
	virtual ~ScriptCommandsSystem();

protected:
	struct ScriptCommandHelper : public BasicScriptCommandHelper {
		ScriptCommandHelper(ScriptCommandsSystem& parent, ska::TileWorld& w, ska::EntityManager& e, PokemonGameEventDispatcher& ged) :
		    BasicScriptCommandHelper(w, e), m_world(w), m_ged(ged) {}

		virtual void setupCommands(ska::TileWorld& w, std::unordered_map<std::string, ska::CommandPtr>& c) const override;
	private:
		ska::TileWorld& m_world;
		PokemonGameEventDispatcher& m_ged;
	};


};

