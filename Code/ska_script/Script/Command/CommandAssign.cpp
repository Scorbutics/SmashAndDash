#include "CommandAssign.h"
#include "../ScriptUtils.h"
#include "../ScriptSymbolsConstants.h"
#include "../System/ScriptAutoSystem.h"

ska::CommandAssign::CommandAssign(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


ska::CommandAssign::~CommandAssign()
{
}

int ska::CommandAssign::argumentsNumber() {
	return 2;
}

std::string ska::CommandAssign::execute(ScriptComponent& script, std::vector<std::string>& args)
{
	std::string valeur, varNumber, line, commandCall;

	valeur = args[0];
	varNumber = args[1];

	ScriptUtils::setValueFromVarOrSwitchNumber(script.parent->getSavegame(), script.extendedName, ScriptSymbolsConstants::VARIABLE_LEFT + varNumber + ScriptSymbolsConstants::VARIABLE_RIGHT, valeur, script.varMap);
	return "";
}
