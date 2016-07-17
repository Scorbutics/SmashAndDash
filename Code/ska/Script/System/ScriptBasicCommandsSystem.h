#pragma once
#include "ScriptAutoSystem.h"

namespace ska {
	class World;
	class ScriptBasicCommandsSystem : public ScriptAutoSystem {
		friend class BasicScriptCommandHelper;
	public :
		ScriptBasicCommandsSystem(World& w, EntityManager& entityManager, ska::Savegame& saveGame);
		virtual ~ScriptBasicCommandsSystem();

	protected:
		ScriptBasicCommandsSystem(World& w, const ScriptCommandHelper& sch, EntityManager& entityManager, ska::Savegame& saveGame);
		struct BasicScriptCommandHelper : public ska::ScriptAutoSystem::ScriptCommandHelper {
			BasicScriptCommandHelper(ska::World& w, EntityManager& entityManager) : ScriptAutoSystem::ScriptCommandHelper(w, entityManager) {}
			virtual void setupCommands(ska::World& w, std::unordered_map<std::string, CommandPtr>& c) const override;
		};
	};

}