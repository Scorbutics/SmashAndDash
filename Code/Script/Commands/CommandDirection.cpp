#include "CommandDirection.h"
#include "Gameplay\WGameCore.h"
#include "Utils\ScriptUtils.h"


CommandDirection::CommandDirection()
{
}


CommandDirection::~CommandDirection()
{
}

int CommandDirection::argumentsNumber() {
	return 3;
}

bool CommandDirection::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	string idTypeStr, idStr, dirStr;
	int dir, id, idType;

	idTypeStr = args[0];
	idStr = args[1];
	dirStr = args[2];

	idType = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, idTypeStr, varMap);
	id = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, idStr, varMap);
	dir = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, dirStr, varMap);

	wScreen.getEntityFactory().getNPC(idType, id)->setDirection(dir);
	return true;
}
