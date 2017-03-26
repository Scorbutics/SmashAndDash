#include "CommandEndScript.h"
#include "Exceptions/ScriptDiedException.h"

ska::CommandEndScript::CommandEndScript(EntityManager& e) : CommandEnd(e) {
}


ska::CommandEndScript::~CommandEndScript() {
}

int ska::CommandEndScript::argumentsNumber() {
	return 1;
}

std::string ska::CommandEndScript::execute(ScriptComponent&, std::vector<std::string>& args) {
	throw ScriptDiedException(args[0]);
}
