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

bool CommandPlayCrumbling::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int duree = 0;

	duree = StringUtils::strToInt(args[0]);

	wScreen.getParticleManager().playCrumbling(1, 30, 0.25, duree);
	return true;
}
