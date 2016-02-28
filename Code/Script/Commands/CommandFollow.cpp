#include "CommandFollow.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"

CommandFollow::CommandFollow()
{
}


CommandFollow::~CommandFollow()
{
}

int CommandFollow::argumentsNumber() {
	return 5;
}

std::string CommandFollow::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int id, number, id_target, number_target, flag;

	id = ska::StringUtils::strToInt(args[0]);
	number = ska::StringUtils::strToInt(args[1]);
	id_target = ska::StringUtils::strToInt(args[2]);
	number_target = ska::StringUtils::strToInt(args[3]);
	flag = ska::StringUtils::strToInt(args[4]);

	wScreen.getEntityFactory().getNPC(id, number)->followEntity(flag ? wScreen.getEntityFactory().getNPC(id_target, number_target) : NULL);
	return "";
}
