#include "CommandWait.h"
#include "Utils/StringUtils.h"
#include "../../Script/ScriptComponent.h"
#include "../../Script/System/ScriptAutoSystem.h"

ska::CommandWait::CommandWait(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


ska::CommandWait::~CommandWait()
{
}

int ska::CommandWait::argumentsNumber() {
	return 1;
}

std::string ska::CommandWait::execute(ScriptComponent& script, std::vector<std::string>& args) {
	unsigned int duree = 0;
	duree = StringUtils::strToInt(args[0]);

	/* Delay the script : assign an uint value > 1 */
	script.active = duree;
	return "";
}
