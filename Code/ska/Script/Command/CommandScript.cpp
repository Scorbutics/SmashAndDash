#include <string>
#include "CommandScript.h"
#include "../../Utils/StringUtils.h"
#include "../../Exceptions/ScriptSyntaxError.h"
#include "../ScriptDispatcher.h"

using namespace std;

ska::CommandScript::CommandScript()
{
}


ska::CommandScript::~CommandScript()
{
}

int ska::CommandScript::argumentsNumber() {
	return -1;
}

std::string ska::CommandScript::execute(ska::IScript* script, std::vector<std::string>& args)
{
	string scriptName;
	vector<string> extraArgs;
	string periodStr;
	
	if (args.size() < 3) {
		throw ska::ScriptSyntaxError("Syntax error with command \"script\" : this command needs at least 3 parameters.");
	}

	scriptName = args[0];
	unsigned int period = ska::StringUtils::strToInt(args[1]);

	/* Rebuild an argument string to be read by the new running script */
	for (unsigned int i = 2; i < args.size(); i++) {
		extraArgs.push_back(args[i]);
	}

	ska::IScript* started = script->getParent().addRunningScript(script, scriptName, script->getContext(), extraArgs, 0, &period);

	return started == NULL ? "" : started->getKey();

}
