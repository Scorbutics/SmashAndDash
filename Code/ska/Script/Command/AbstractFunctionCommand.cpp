#include "AbstractFunctionCommand.h"
#include "..\..\Utils\StringUtils.h"
#include "..\..\Exceptions\ScriptSyntaxError.h"
#include "../ScriptSymbolsConstants.h"
#include "../ScriptComponent.h"

using namespace std;

ska::AbstractFunctionCommand::AbstractFunctionCommand(EntityManager& entityManager) : AbstractCommand(entityManager)
{
}

std::string ska::AbstractFunctionCommand::process(ScriptComponent& script, std::stringstream& streamCmd, std::vector<std::string>& args) {
	int argNumber = argumentsNumber();
	if (argNumber != -1 && argNumber != args.size()) {
		/* Syntax error */
		std::string syntaxErrorMsg("Syntax error with parameters : ");
		for (std::string& arg : args) {
			syntaxErrorMsg += arg + " ";
		}
		throw ska::ScriptSyntaxError(syntaxErrorMsg);
	}

	return execute(script, args);
}

char ska::AbstractFunctionCommand::getSeparator() {
	return ScriptSymbolsConstants::ARGUMENT_SEPARATOR;
}

ska::AbstractFunctionCommand::~AbstractFunctionCommand()
{
}
