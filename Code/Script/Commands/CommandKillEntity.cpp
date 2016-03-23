#include "CommandKillEntity.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils\StringUtils.h"
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

std::string CommandKillEntity::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int id, number;
	string param;

	id = ska::StringUtils::strToInt(args[0]);
	number = ska::StringUtils::strToInt(args[1]);

	//wScreen.getEntityFactory().remove(id, number);
	return "";
}
