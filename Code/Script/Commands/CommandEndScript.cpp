#include "CommandEndScript.h"


CommandEndScript::CommandEndScript() {
}


CommandEndScript::~CommandEndScript() {
}

int CommandEndScript::argumentsNumber() {
	return 2;
}

std::string CommandEndScript::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) {
	//return CommandEnd::execute(args[1], args, scriptList, varMap, fscript, active, result);
	return "";
}