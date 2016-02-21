#include "CommandScript.h"
#include <string>
#include "..\ScriptDispatcher.h"
#include "../../Utils\StringUtils.h"
#include "../../Exceptions/ScriptSyntaxError.h"

using namespace std;

CommandScript::CommandScript()
{
}


CommandScript::~CommandScript()
{
}

int CommandScript::argumentsNumber() {
	return -1;
}

std::string CommandScript::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	string scriptName;
	string extraArgs;
	string periodStr;
	
	if (args.size() < 3) {
		throw ScriptSyntaxError("Syntax error with command \"script\" : this command needs at least 3 parameters.");
	}

	scriptName = args[0];
	Uint32 period = StringUtils::strToInt(args[1]);
	extraArgs = args[2];

	/* Rebuild an argument string to be read by the new running script */
	for (unsigned int i = 3; i < args.size(); i++) {
		extraArgs += getSeparator() + args[i];
	}

	IScript* started = ScriptDispatcher::getInstance().addRunningScript(script, scriptName, extraArgs, 0, &period);

	return started == NULL ? "" : started->getKey();

}
