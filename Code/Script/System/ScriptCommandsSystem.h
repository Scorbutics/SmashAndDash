#pragma once
#include "../../ska/Script/System/ScriptBasicCommandsSystem.h"
#include "../../ska/Scene/SceneSwitcher.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "../../ska/Utils/Observable.h"
#include "../../ska/Utils/Observer.h"
#include "../../Gameplay/PokemonGameEventDispatcherDeclaration.h"

class WorldScene;

namespace ska {
	class World;
}

using SceneChangeObservable = ska::Observable<ska::SceneSwitcher<WorldScene&>>;

class ScriptCommandsSystem :
	public ska::ScriptBasicCommandsSystem, public SceneChangeObservable
{
public:
	ScriptCommandsSystem(ska::World& w, ska::EntityManager& entityManager, ska::Savegame& saveGame, PokemonGameEventDispatcher& ged);
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

