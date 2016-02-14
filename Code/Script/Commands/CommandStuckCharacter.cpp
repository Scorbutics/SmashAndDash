#include "CommandStuckCharacter.h"
#include "Gameplay\WGameCore.h"
#include "Utils\StringUtils.h"

CommandStuckCharacter::CommandStuckCharacter()
{
}


CommandStuckCharacter::~CommandStuckCharacter()
{
}

int CommandStuckCharacter::argumentsNumber() {
	return 3;
}

bool CommandStuckCharacter::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int id, number, moving;

	id = StringUtils::strToInt(args[0]);
	number = StringUtils::strToInt(args[1]);
	moving = StringUtils::strToInt(args[2]);

	wScreen.getEntityFactory().getNPC(id, number)->setMoving(!moving);
	return true;
}
