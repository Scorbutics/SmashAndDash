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

std::string CommandAssign::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	string valeur, varNumber, line, commandCall;
	size_t commandCallSize;

	valeur = args[0];
	varNumber = args[1];

	ScriptUtils::setValueFromVarOrSwitchNumber(script->getExtendedName(), ScriptSymbolsConstants::VARIABLE_LEFT + varNumber + ScriptSymbolsConstants::VARIABLE_RIGHT, valeur, script->getVarMap());
	return "";
}
