#include "CommandAssign.h"
#include <map>
#include "../../Utils\ScriptUtils.h"
#include "../ScriptSymbolsConstants.h"

using namespace std;

CommandAssign::CommandAssign()
{
}


CommandAssign::~CommandAssign()
{
}

int CommandAssign::argumentsNumber() {
	return 2;
}

bool CommandAssign::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	string valeur, varNumber, line, commandCall;
	size_t commandCallSize;

	valeur = args[0];
	varNumber = args[1];

	ScriptUtils::setValueFromVarOrSwitchNumber(extendedName, ScriptSymbolsConstants::VARIABLE_LEFT + varNumber + ScriptSymbolsConstants::VARIABLE_RIGHT, valeur, varMap);
	return true;
}
