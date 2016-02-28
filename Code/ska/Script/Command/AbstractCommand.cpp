#include <iostream>
#include "AbstractCommand.h"
#include "..\..\Utils\StringUtils.h"
#include "..\..\Utils\ScriptUtils.h"
#include "../ScriptSymbolsConstants.h"
#include "../IScript.h"

using namespace std;

ska::AbstractCommand::AbstractCommand()
{
}

std::string ska::AbstractCommand::process(IScript* script, stringstream& streamCmd, ofstream& scriptList) {

	string line;
	getline(streamCmd, line);

	line = ska::StringUtils::trim(line);

	/* Avant tout traitement, effectue toutes les ex�cutions des sous-commandes possibles */
	interpretSubCommands(line, script);

	vector<string>& args = ska::StringUtils::split(line, getSeparator());
	
	for (string& arg : args) {
		arg = ska::StringUtils::trim(arg);
		/* Pour chaque argument, explicite les valeurs des variables */
		arg = ScriptUtils::getValueFromVarOrSwitchNumber(script->getExtendedName(), arg, script->getVarMap());
	}
	

	return process(script, streamCmd, args, scriptList);
}

std::string ska::AbstractCommand::interpretSubCommands(string& line, IScript* script) {
	size_t outputCommandSize = 0;
	size_t offset;
	string parsedArg;
	
	/* Conservation de tout ce qui se trouve sur la ligne avant l'appel � la sous-commande */
	size_t offsetLineCmd = line.find_first_of(ska::ScriptSymbolsConstants::METHOD);
	string firstLinePart = (offsetLineCmd == std::string::npos ? "" : line.substr(0, offsetLineCmd));
	parsedArg += firstLinePart;

	do {
		string expression = line.substr(outputCommandSize);

		parsedArg += ScriptUtils::getFirstExpressionFromLine(expression, script, &offset);
		parsedArg += " ";
		outputCommandSize += offset;
	} while (offset != 0 && outputCommandSize < line.size());
	line = ska::StringUtils::rtrim(parsedArg);
	return "";
}

ska::AbstractCommand::~AbstractCommand()
{
}