#include "CommandHideCharacter.h"
#include "../../Gameplay/WGameCore.h"
#include "../../ska/Utils/StringUtils.h"

CommandHideCharacter::CommandHideCharacter()
{
}


CommandHideCharacter::~CommandHideCharacter()
{
}

int CommandHideCharacter::argumentsNumber() {
	return 3;
}

std::string CommandHideCharacter::execute(ska::IScript* script, std::vector<std::string>& args)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int id, number, hiding;

	id = ska::StringUtils::strToInt(args[0]);
	number = ska::StringUtils::strToInt(args[1]);
	hiding = ska::StringUtils::strToInt(args[2]);

	//wScreen.getEntityFactory().getNPC(id, number)->setVisible(!hiding);
	return "";
}
