#include "CommandStuckCharacter.h"
#include "Utils/StringUtils.h"

CommandStuckCharacter::CommandStuckCharacter(ska::EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int CommandStuckCharacter::argumentsNumber() {
	return 3;
}

std::string CommandStuckCharacter::execute(ska::ScriptComponent&, ska::MemoryScript& memoryScript, const std::vector<std::string>& ) {
	//WGameCore& wScreen = WGameCore::getInstance();
	/*int id, number, moving;

	id = ska::StringUtils::strToInt(args[0]);
	number = ska::StringUtils::strToInt(args[1]);
	moving = ska::StringUtils::strToInt(args[2]);*/

	//wScreen.getEntityFactory().getNPC(id, number)->setMoving(!moving);
	return "";
}
