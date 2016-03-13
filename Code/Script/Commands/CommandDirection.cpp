#include "CommandDirection.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils\StringUtils.h"


CommandDirection::CommandDirection()
{
}


CommandDirection::~CommandDirection()
{
}

int CommandDirection::argumentsNumber() {
	return 3;
}

std::string CommandDirection::execute(ska::IScript* script, std::vector<std::string>& args)
{
	WGameCore& wScreen = WGameCore::getInstance();
	string idTypeStr, idStr, dirStr;
	int dir, id, idType;

	idTypeStr = args[0];
	idStr = args[1];
	dirStr = args[2];

	idType = ska::StringUtils::strToInt(idTypeStr);
	id = ska::StringUtils::strToInt(idStr);
	dir = ska::StringUtils::strToInt(dirStr);

	//wScreen.getEntityFactory().getNPC(idType, id)->setDirection(dir);
	return "";
}
