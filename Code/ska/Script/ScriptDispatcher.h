#pragma once
#include "Script.h"
#include <string>
#include <unordered_map>
#include <map>
#include "./Command/Command.h"
#include "../Utils/Singleton_template.h"
#include "../Data/Savegame.h"

namespace ska {
	class ScriptDispatcher {;
	public:
		ScriptDispatcher(ska::Savegame& saveGame);
		IScript* addRunningScript(IScript* parent, const std::string& name, const std::string& context, const std::vector<std::string>& args, const int triggeringType, const unsigned int* period = NULL);
		void setupScriptArgs(IScript* parent, IScript* script, const std::vector<std::string>& args);
		void kill(const std::string& keyScript);
		void refresh();
		~ScriptDispatcher();
		std::string commandInterpreter(IScript* script, const std::string& cmd);

		void clear();
	private:
		ska::Savegame& m_saveGame;
		IScript* getHighestPriorityScript();
		std::unordered_map<std::string, IScriptPtr> m_scripts;
	};
}
