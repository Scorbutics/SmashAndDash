#pragma once
#include "../../ska/Script/System/ScriptBasicCommandsSystem.h"

class ScriptCommandsSystem :
	public ska::ScriptBasicCommandsSystem
{
public:
	ScriptCommandsSystem(ska::EntityManager& entityManager, ska::Savegame& saveGame);
	virtual ~ScriptCommandsSystem();

protected:
	struct ScriptCommandHelper : public ska::ScriptBasicCommandsSystem::BasicScriptCommandHelper {
		ScriptCommandHelper(ska::EntityManager& e) : ScriptBasicCommandsSystem::BasicScriptCommandHelper(e) {}
		virtual void setupCommands(std::unordered_map<std::string, ska::CommandPtr>& c) const override;
	};
};

