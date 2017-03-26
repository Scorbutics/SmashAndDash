#include "CommandPlayCrumbling.h"
//#include "../../Gameplay\WGameCore.h"
#include "Utils/StringUtils.h"

CommandPlayCrumbling::CommandPlayCrumbling(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandPlayCrumbling::~CommandPlayCrumbling()
{
}

int CommandPlayCrumbling::argumentsNumber() {
	return 1;
}

std::string CommandPlayCrumbling::execute(ska::ScriptComponent&, std::vector<std::string>& )
{
	//WGameCore& wScreen = WGameCore::getInstance();
	/*int duree = 0;

	duree = ska::StringUtils::strToInt(args[0]);*/

	//wScreen.getParticleManager().playCrumbling(1, 30, 0.25, duree);
	return "";
}
