#include <iostream>
#include "CommandLog.h"
#include "Logging/Logger.h"

ska::CommandLog::CommandLog(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}

std::string ska::CommandLog::execute(ScriptComponent&, std::vector<std::string>& args) {

	std::string& message = args[0];

	SKA_LOG_MESSAGE(message);

	return "";
}

int ska::CommandLog::argumentsNumber() {
	return 1;
}

ska::CommandLog::~CommandLog()
{
}
