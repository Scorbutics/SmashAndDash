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
	vector<string> extraArgs;
	string periodStr;
	
	if (args.size() < 3) {
		throw ska::ScriptSyntaxError("Syntax error with command \"script\" : this command needs at least 3 parameters.");
	}

	scriptName = args[0];
	Uint32 period = ska::StringUtils::strToInt(args[1]);

	/* Rebuild an argument string to be read by the new running script */
	for (unsigned int i = 2; i < args.size(); i++) {
		extraArgs.push_back(args[i]);
	}

	IScript* started = ScriptDispatcher::getInstance().addRunningScript(script, scriptName, extraArgs, 0, &period);

	return started == NULL ? "" : started->getKey();

}
