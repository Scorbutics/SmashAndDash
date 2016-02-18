#include "CommandPlayCrumbling.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"

CommandPlayCrumbling::CommandPlayCrumbling()
{
}


CommandPlayCrumbling::~CommandPlayCrumbling()
{
}

int CommandPlayCrumbling::argumentsNumber() {
	return 1;
}

std::string CommandPlayCrumbling::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int duree = 0;

	duree = StringUtils::strToInt(args[0]);

	wScreen.getParticleManager().playCrumbling(1, 30, 0.25, duree);
	return "";
}
