#include "CommandEndScript.h"
#include "../ScriptDispatcher.h"

CommandEndScript::CommandEndScript() {
}


CommandEndScript::~CommandEndScript() {
}

int CommandEndScript::argumentsNumber() {
	return 1;
}

std::string CommandEndScript::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) {
	ScriptDispatcher::getInstance().kill(args[0]);
	return "";
}