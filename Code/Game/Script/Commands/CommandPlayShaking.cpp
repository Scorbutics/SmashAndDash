#include "CommandPlayShaking.h"
#include "Utils/StringUtils.h"

CommandPlayShaking::CommandPlayShaking(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandPlayShaking::~CommandPlayShaking()
{
}

int CommandPlayShaking::argumentsNumber() {
	return 2;
}

std::string CommandPlayShaking::execute(ska::ScriptComponent&, ska::MemoryScript& memory, std::vector<std::string>& )
{
	//TODO Use a ShakerSystem

	//WGameCore& wScreen = WGameCore::getInstance();
	/*unsigned intensity, duration;

	duration = ska::StringUtils::strToInt(args[0]);
	intensity = ska::StringUtils::strToInt(args[1]);*/

	//wScreen.getShakerManager().shake(intensity, duration);
	return "";
}
