#include "CommandKillEntity.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"
#include <string>

using namespace std;

CommandKillEntity::CommandKillEntity()
{
}


CommandKillEntity::~CommandKillEntity()
{
}

int CommandKillEntity::argumentsNumber() {
	return 2;
}

bool CommandKillEntity::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int id, number;
	string param;

	id = StringUtils::strToInt(args[0]);
	number = StringUtils::strToInt(args[1]);

	wScreen.getEntityFactory().remove(id, number);
	return true;
}
