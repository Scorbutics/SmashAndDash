#include "AbstractFunctionCommand.h"
#include "..\..\Utils\StringUtils.h"
#include "..\..\Exceptions\ScriptSyntaxError.h"
#include "../ScriptSymbolsConstants.h"

using namespace std;

AbstractFunctionCommand::AbstractFunctionCommand()
{
}

std::string AbstractFunctionCommand::process(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList) {
	int argNumber = argumentsNumber();
	if (argNumber != -1 && argNumber != args.size()) {
		/* Syntax error */
		std::string syntaxErrorMsg("[" + script->getExtendedName() + "] Syntax error with parameters : ");
		for (std::string& arg : args) {
			syntaxErrorMsg += arg + " ";
		}
		throw ScriptSyntaxError(syntaxErrorMsg);
	}

	return execute(script, args, scriptList);
}

char AbstractFunctionCommand::getSeparator() {
	return ScriptSymbolsConstants::ARGUMENT_SEPARATOR;
}

AbstractFunctionCommand::~AbstractFunctionCommand()
{
}
