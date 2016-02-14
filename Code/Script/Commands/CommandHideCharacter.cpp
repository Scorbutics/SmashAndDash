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

bool CommandHideCharacter::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int id, number, hiding;

	id = StringUtils::strToInt(args[0]);
	number = StringUtils::strToInt(args[1]);
	hiding = StringUtils::strToInt(args[2]);

	wScreen.getEntityFactory().getNPC(id, number)->setVisible(!hiding);
	return true;
}
