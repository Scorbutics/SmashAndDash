#pragma once
#include "ScriptAutoSystem.h"

namespace ska {
	class World;
	class ScriptBasicCommandsSystem : public ScriptAutoSystem {
		friend class BasicScriptCommandHelper;
	public :
		ScriptBasicCommandsSystem(World& w, EntityManager& entityManager, Savegame& saveGame);
		virtual ~ScriptBasicCommandsSystem();

	protected:
		ScriptBasicCommandsSystem(World& w, const ScriptCommandHelper& sch, EntityManager& entityManager, Savegame& saveGame);
		struct BasicScriptCommandHelper : public ScriptCommandHelper {
			BasicScriptCommandHelper(World& w, EntityManager& entityManager) : ScriptCommandHelper(w, entityManager) {}
			void operator=(const BasicScriptCommandHelper&) = delete;
			virtual void setupCommands(World& w, std::unordered_map<std::string, CommandPtr>& c) const override;
		};
	};

}
