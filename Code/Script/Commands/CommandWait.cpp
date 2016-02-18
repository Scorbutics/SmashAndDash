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

std::string CommandWait::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	unsigned int duree = 0, t0 = 0;
	WGameCore& wScreen = WGameCore::getInstance();
	duree = StringUtils::strToInt(args[0]);

	/* Delay the script : assign an uint value > 1 */
	script->delay(duree);
	return "";
}
