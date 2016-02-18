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

std::string CommandScript::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	string scriptName;
	string extraArgs;
	string periodStr;
	
	scriptName = args[0];
	Uint32 period = StringUtils::strToInt(args[1]);
	extraArgs = args[2];

	ScriptDispatcher::getInstance().addRunningScript(script, scriptName, extraArgs, 0, &period);

	return scriptName;

}
