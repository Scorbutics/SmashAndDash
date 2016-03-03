#include "CommandStuckCharacter.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils\StringUtils.h"

CommandStuckCharacter::CommandStuckCharacter()
{
}


CommandStuckCharacter::~CommandStuckCharacter()
{
}

int CommandStuckCharacter::argumentsNumber() {
	return 3;
}

std::string CommandStuckCharacter::execute(ska::IScript* script, std::vector<std::string>& args)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int id, number, moving;

	id = ska::StringUtils::strToInt(args[0]);
	number = ska::StringUtils::strToInt(args[1]);
	moving = ska::StringUtils::strToInt(args[2]);

	wScreen.getEntityFactory().getNPC(id, number)->setMoving(!moving);
	return "";
}
