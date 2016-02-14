#include "CommandScript.h"
#include <string>
#include "..\ScriptDispatcher.h"
#include "../../Utils\StringUtils.h"

using namespace std;

CommandScript::CommandScript()
{
}


CommandScript::~CommandScript()
{
}

int CommandScript::argumentsNumber() {
	return 3;
}

bool CommandScript::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	string scriptName;
	string extraArgs;
	string periodStr;
	
	scriptName = args[0];
	Uint32 period = StringUtils::strToInt(args[1]);
	extraArgs = args[2];

	active = false;

	ScriptDispatcher::getInstance().addRunningScript(scriptName, extraArgs.substr(1, extraArgs.length()), 0, &period);

	active = true;
	return true;
}
