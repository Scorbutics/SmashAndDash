#include "CommandPlayShaking.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils/StringUtils.h"

CommandPlayShaking::CommandPlayShaking(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandPlayShaking::~CommandPlayShaking()
{
}

int CommandPlayShaking::argumentsNumber() {
	return 2;
}

std::string CommandPlayShaking::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	WGameCore& wScreen = WGameCore::getInstance();
	unsigned intensity, duration;

	duration = ska::StringUtils::strToInt(args[0]);
	intensity = ska::StringUtils::strToInt(args[1]);

	wScreen.getShakerManager().shake(intensity, duration);
	return "";
}
