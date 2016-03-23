#pragma once
#include <unordered_set>
#include <string>
#include <unordered_map>
#include "../Script.h"
#include "../Command/Command.h"
#include "../../Data/Savegame.h"
#include "../../ECS/System.h"

namespace ska {
	class ScriptAutoSystem : public System<std::unordered_set<EntityId>, ScriptComponent>
	{
	public:
		ScriptAutoSystem(ska::Savegame& saveGame);
		
		Savegame& getSavegame();
		IScript* addRunningScript(IScript* parent, const std::string& name, const std::string& context, const std::vector<std::string>& args, const int triggeringType, const unsigned int* period = NULL);
		void setupScriptArgs(IScript* parent, IScript* script, const std::vector<std::string>& args);
		void kill(const std::string& keyScript);
		virtual void refresh() override;
		std::string commandInterpreter(IScript* script, const std::string& cmd);
		void clear();

		virtual ~ScriptAutoSystem();

	private:
		ska::Savegame& m_saveGame;
		IScript* getHighestPriorityScript();
		std::unordered_map<std::string, IScriptPtr> m_scripts;
		
	};

}
