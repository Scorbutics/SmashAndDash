#include "CommandPlayCrumbling.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils\StringUtils.h"

CommandPlayCrumbling::CommandPlayCrumbling()
{
}


CommandPlayCrumbling::~CommandPlayCrumbling()
{
}

int CommandPlayCrumbling::argumentsNumber() {
	return 1;
}

std::string CommandPlayCrumbling::execute(ska::IScript* script, std::vector<std::string>& args)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int duree = 0;

	duree = ska::StringUtils::strToInt(args[0]);

	wScreen.getParticleManager().playCrumbling(1, 30, 0.25, duree);
	return "";
}
