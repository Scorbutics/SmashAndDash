#include "CommandHideCharacter.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"

CommandHideCharacter::CommandHideCharacter()
{
}


CommandHideCharacter::~CommandHideCharacter()
{
}

int CommandHideCharacter::argumentsNumber() {
	return 3;
}

std::string CommandHideCharacter::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int id, number, hiding;

	id = StringUtils::strToInt(args[0]);
	number = StringUtils::strToInt(args[1]);
	hiding = StringUtils::strToInt(args[2]);

	wScreen.getEntityFactory().getNPC(id, number)->setVisible(!hiding);
	return "";
}
