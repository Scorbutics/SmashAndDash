#include "CommandEndScript.h"
#include "../ScriptDispatcher.h"
#include "../../Exceptions/ScriptDiedException.h"

ska::CommandEndScript::CommandEndScript() {
}


ska::CommandEndScript::~CommandEndScript() {
}

int ska::CommandEndScript::argumentsNumber() {
	return 1;
}

std::string ska::CommandEndScript::execute(ScriptComponent& script, std::vector<std::string>& args) {
	throw ska::ScriptDiedException(args[0]);
}