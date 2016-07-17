#pragma once
#include "../../ska/Script/System/ScriptBasicCommandsSystem.h"

namespace ska {
	class World;
}

class ScriptCommandsSystem :
	public ska::ScriptBasicCommandsSystem
{
public:
	ScriptCommandsSystem(ska::World& w, ska::EntityManager& entityManager, ska::Savegame& saveGame);
	virtual ~ScriptCommandsSystem();

protected:
	struct ScriptCommandHelper : public ska::ScriptBasicCommandsSystem::BasicScriptCommandHelper {
		ScriptCommandHelper(ska::World& w, ska::EntityManager& e) : ScriptBasicCommandsSystem::BasicScriptCommandHelper(w, e), m_world(w) {}
		virtual void setupCommands(ska::World& w, std::unordered_map<std::string, ska::CommandPtr>& c) const override;
	private:
		ska::World& m_world;
	};


};

