#include <iostream>
#include "CommandLog.h"

ska::CommandLog::CommandLog(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}

std::string ska::CommandLog::execute(ScriptComponent& script, std::vector<std::string>& args) {

	std::string& message = args[0];

	std::clog << message << std::endl;

	return "";
}

int ska::CommandLog::argumentsNumber() {
	return 1;
}

ska::CommandLog::~CommandLog()
{
}
