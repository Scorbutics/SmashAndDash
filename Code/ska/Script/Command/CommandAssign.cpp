#include "CommandAssign.h"
#include "../../Utils\ScriptUtils.h"
#include "../ScriptSymbolsConstants.h"
#include "../ScriptDispatcher.h"

using namespace std;

ska::CommandAssign::CommandAssign()
{
}


ska::CommandAssign::~CommandAssign()
{
}

int ska::CommandAssign::argumentsNumber() {
	return 2;
}

std::string ska::CommandAssign::execute(IScript* script, std::vector<std::string>& args)
{
	string valeur, varNumber, line, commandCall;

	valeur = args[0];
	varNumber = args[1];

	ScriptUtils::setValueFromVarOrSwitchNumber(script->getParent().getSavegame(), script->getExtendedName(), ScriptSymbolsConstants::VARIABLE_LEFT + varNumber + ScriptSymbolsConstants::VARIABLE_RIGHT, valeur, script->getVarMap());
	return "";
}
