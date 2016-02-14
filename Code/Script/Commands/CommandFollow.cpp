#include "CommandFollow.h"
#include "Gameplay\WGameCore.h"
#include "Utils\StringUtils.h"

CommandFollow::CommandFollow()
{
}


CommandFollow::~CommandFollow()
{
}

int CommandFollow::argumentsNumber() {
	return 5;
}

bool CommandFollow::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int id, number, id_target, number_target, flag;

	id = StringUtils::strToInt(args[0]);
	number = StringUtils::strToInt(args[1]);
	id_target = StringUtils::strToInt(args[2]);
	number_target = StringUtils::strToInt(args[3]);
	flag = StringUtils::strToInt(args[4]);

	wScreen.getEntityFactory().getNPC(id, number)->followEntity(flag ? wScreen.getEntityFactory().getNPC(id_target, number_target) : NULL);
	return true;
}
