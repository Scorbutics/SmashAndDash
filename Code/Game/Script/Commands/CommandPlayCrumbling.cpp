#include "CommandPlayCrumbling.h"
#include "Utils/StringUtils.h"

CommandPlayCrumbling::CommandPlayCrumbling(ska::EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int CommandPlayCrumbling::argumentsNumber() {
	return 1;
}

std::string CommandPlayCrumbling::execute(ska::ScriptComponent&, ska::MemoryScript& memory, const std::vector<std::string>& ) {
	//WGameCore& wScreen = WGameCore::getInstance();
	/*int duree = 0;

	duree = ska::StringUtils::strToInt(args[0]);*/

	//wScreen.getParticleManager().playCrumbling(1, 30, 0.25, duree);
	return "";
}
