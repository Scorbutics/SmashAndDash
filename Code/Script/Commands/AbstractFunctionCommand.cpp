#include "AbstractFunctionCommand.h"
#include "..\..\Utils\StringUtils.h"
#include "..\..\Exceptions\ScriptSyntaxError.h"
#include "../ScriptSymbolsConstants.h"

using namespace std;

AbstractFunctionCommand::AbstractFunctionCommand()
{
}

bool AbstractFunctionCommand::process(const std::string& extendedName, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) {
	int argNumber = argumentsNumber();
	if (argNumber != args.size()) {
		/* Syntax error */
		std::string syntaxErrorMsg("[" + extendedName + "] Syntax error with parameters : ");
		for (std::string& arg : args) {
			syntaxErrorMsg += arg + " ";
		}
		throw ScriptSyntaxError(syntaxErrorMsg);
	}

	return execute(extendedName, args, scriptList, varMap, fscript, active, result);
}

char AbstractFunctionCommand::getSeparator() {
	return ScriptSymbolsConstants::ARGUMENT_SEPARATOR;
}

AbstractFunctionCommand::~AbstractFunctionCommand()
{
}
