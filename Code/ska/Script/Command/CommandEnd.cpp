#include "CommandEnd.h"
#include "../../Utils\ScriptUtils.h"
#include "../../Utils\StringUtils.h"
#include "../../Utils\SkaConstants.h"
#include "../../Exceptions/ScriptDiedException.h"

ska::CommandEnd::CommandEnd(EntityManager& e) : AbstractFunctionCommand(e)
{
}


ska::CommandEnd::~CommandEnd()
{
}

int ska::CommandEnd::argumentsNumber() {
	return 0;
}

const std::string& ska::CommandEnd::getCmdName() {
	static const std::string cmdName = "end";
	return cmdName;
}

std::string ska::CommandEnd::execute(ScriptComponent& script, std::vector<std::string>& args) {
	throw ska::ScriptDiedException("");
}
