#include "CommandPlayShaking.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"

CommandPlayShaking::CommandPlayShaking()
{
}


CommandPlayShaking::~CommandPlayShaking()
{
}

int CommandPlayShaking::argumentsNumber() {
	return 2;
}

std::string CommandPlayShaking::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	WGameCore& wScreen = WGameCore::getInstance();
	unsigned intensity, duration;

	duration = StringUtils::strToInt(args[0]);
	intensity = StringUtils::strToInt(args[1]);

	wScreen.getShakerManager().shake(intensity, duration);
	return "";
}
