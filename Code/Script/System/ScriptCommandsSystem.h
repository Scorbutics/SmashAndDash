#pragma once
#include "../../ska/Script/System/ScriptBasicCommandsSystem.h"
#include "../../ska/Scene/SceneSwitcher.h"
#include "../../ska/Utils/Observable.h"
#include "../../ska/Utils/Observer.h"

class WorldScene;

namespace ska {
	class World;
}

using SceneChangeObservable = ska::Observable<ska::SceneSwitcher<WorldScene&>>;

class ScriptCommandsSystem :
	public ska::ScriptBasicCommandsSystem, public SceneChangeObservable
{
public:
	ScriptCommandsSystem(ska::World& w, ska::EntityManager& entityManager, ska::Savegame& saveGame);
	virtual ~ScriptCommandsSystem();

protected:
	struct ScriptCommandHelper : public ska::ScriptBasicCommandsSystem::BasicScriptCommandHelper {
		ScriptCommandHelper(ScriptCommandsSystem& parent, ska::World& w, ska::EntityManager& e) : ScriptBasicCommandsSystem::BasicScriptCommandHelper(w, e), m_world(w), m_parent(parent) {}
		virtual void setupCommands(ska::World& w, std::unordered_map<std::string, ska::CommandPtr>& c) const override;
	private:
		ska::World& m_world;
		ScriptCommandsSystem& m_parent;
	};


};

