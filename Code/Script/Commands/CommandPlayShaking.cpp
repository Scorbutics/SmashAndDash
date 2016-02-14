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

bool CommandPlayShaking::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	unsigned intensity, duration;

	duration = StringUtils::strToInt(args[0]);
	intensity = StringUtils::strToInt(args[1]);

	wScreen.getShakerManager().shake(intensity, duration);
	return true;
}
