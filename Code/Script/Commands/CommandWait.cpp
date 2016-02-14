#include "CommandWait.h"
#include <map>
#include <SDL2\SDL_timer.h>
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"

CommandWait::CommandWait()
{
}


CommandWait::~CommandWait()
{
}

int CommandWait::argumentsNumber() {
	return 1;
}

bool CommandWait::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	unsigned int duree = 0, t0 = 0;
	WGameCore& wScreen = WGameCore::getInstance();
	duree = StringUtils::strToInt(args[0]);

	/* Delay the script : assign an uint value > 1 to "active" */
	active = duree;
	return true;
}
