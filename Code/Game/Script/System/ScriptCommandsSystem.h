#pragma once
#include "Script/System/ScriptBasicCommandsSystem.h"
//#include "Core/Scene/SceneSwitcher.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "Utils/Observable.h"

class WorldScene;

namespace ska {
	class World;
}

class ScriptCommandsSystem :
	public ska::ScriptBasicCommandsSystem
{
public:
	ScriptCommandsSystem(ska::EntityManager& entityManager, ska::World& w, ska::Savegame& saveGame, PokemonGameEventDispatcher& ged);
	virtual ~ScriptCommandsSystem();

protected:
	struct ScriptCommandHelper : public BasicScriptCommandHelper {
		ScriptCommandHelper(ScriptCommandsSystem& parent, ska::World& w, ska::EntityManager& e, PokemonGameEventDispatcher& ged) :
		    BasicScriptCommandHelper(w, e), m_world(w), m_ged(ged), m_parent(parent) {}

		virtual void setupCommands(ska::World& w, std::unordered_map<std::string, ska::CommandPtr>& c) const override;
	private:
		ska::World& m_world;
		PokemonGameEventDispatcher& m_ged;
		ScriptCommandsSystem& m_parent;
	};


};

