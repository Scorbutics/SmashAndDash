#include "CommandStuckCharacter.h"
#include "../../ska/Utils/StringUtils.h"

CommandStuckCharacter::CommandStuckCharacter(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandStuckCharacter::~CommandStuckCharacter()
{
}

int CommandStuckCharacter::argumentsNumber() {
	return 3;
}

std::string CommandStuckCharacter::execute(ska::ScriptComponent&, std::vector<std::string>& )
{
	//WGameCore& wScreen = WGameCore::getInstance();
	/*int id, number, moving;

	id = ska::StringUtils::strToInt(args[0]);
	number = ska::StringUtils::strToInt(args[1]);
	moving = ska::StringUtils::strToInt(args[2]);*/

	//wScreen.getEntityFactory().getNPC(id, number)->setMoving(!moving);
	return "";
}
