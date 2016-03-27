#pragma once
#include "ScriptAutoSystem.h"

namespace ska {

	class ScriptBasicCommandsSystem : public ScriptAutoSystem {
		friend class BasicScriptCommandHelper;
	public :
		ScriptBasicCommandsSystem(EntityManager& entityManager, ska::Savegame& saveGame);
		virtual ~ScriptBasicCommandsSystem();

	protected:
		ScriptBasicCommandsSystem(const ScriptCommandHelper& sch, EntityManager& entityManager, ska::Savegame& saveGame);
		struct BasicScriptCommandHelper : public ska::ScriptAutoSystem::ScriptCommandHelper {
			BasicScriptCommandHelper(EntityManager& entityManager) : ScriptAutoSystem::ScriptCommandHelper(entityManager) {}
			virtual void setupCommands(std::unordered_map<std::string, CommandPtr>& c) const override;
		};
	};

}