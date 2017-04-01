#pragma once
#include "ScriptAutoSystem.h"

namespace ska {
	class World;
	class ScriptBasicCommandsSystem : public ScriptAutoSystem {
		friend class BasicScriptCommandHelper;
	public :
		ScriptBasicCommandsSystem(EntityManager& entityManager, World& w, Savegame& saveGame);
		virtual ~ScriptBasicCommandsSystem();

	protected:
		ScriptBasicCommandsSystem(EntityManager& entityManager, World& w, const ScriptCommandHelper& sch, Savegame& saveGame);
		struct BasicScriptCommandHelper : public ScriptCommandHelper {
		    BasicScriptCommandHelper() = default;
		    virtual ~BasicScriptCommandHelper() = default;
			BasicScriptCommandHelper(World& w, EntityManager& entityManager) : ScriptCommandHelper(w, entityManager) {}
			void operator=(const BasicScriptCommandHelper&) = delete;
			virtual void setupCommands(World& w, std::unordered_map<std::string, CommandPtr>& c) const override;
		};
	};

}
