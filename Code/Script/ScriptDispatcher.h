#pragma once
#include "Script.h"
#include <string>
#include <unordered_map>
#include <map>
#include "Commands\Command.h"
#include "../Utils\Singleton_template.h"

class ScriptDispatcher : public Singleton<ScriptDispatcher>{
	friend class Singleton<ScriptDispatcher>;
public:
	ScriptDispatcher();
	int addRunningScript(IScript* parent, std::string name, std::string args, int triggeringType, Uint32* period = NULL);
	void setupScriptArgs(IScript* script, const std::string& extendedName, std::string& args);
	void refresh();
	~ScriptDispatcher();
	static std::string commandInterpreter(IScript* script, const std::string& cmd, std::ifstream& fscript);

	void clear();
private:
	IScript* getHighestPriorityScript();
	static std::unordered_map<std::string, Command_ptr> m_commands;
	std::unordered_map<std::string, IScript_ptr> m_scripts;
};

