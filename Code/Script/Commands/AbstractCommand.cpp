#include <iostream>
#include "AbstractCommand.h"
#include "..\..\Utils\StringUtils.h"
#include "..\..\Utils\ScriptUtils.h"
#include "../ScriptSymbolsConstants.h"

using namespace std;

AbstractCommand::AbstractCommand()
{
}

std::string AbstractCommand::process(IScript* script, stringstream& streamCmd, ofstream& scriptList) {

	string line;
	getline(streamCmd, line);

	line = StringUtils::trim(line);

	/* Avant tout traitement, effectue toutes les exécutions des sous-commandes possibles */
	interpretSubCommands(line, script);

	vector<string>& args = StringUtils::split(line, getSeparator());
	
	for (string& arg : args) {
		arg = StringUtils::trim(arg);
		/* Pour chaque argument, explicite les valeurs des variables */
		arg = ScriptUtils::getValueFromVarOrSwitchNumber(script->getExtendedName(), arg, script->getVarMap());
	}
	

	return process(script, streamCmd, args, scriptList);
}

std::string AbstractCommand::interpretSubCommands(string& line, IScript* script) {
	size_t outputCommandSize = 0;
	size_t offset;
	string parsedArg;
	
	/* Conservation de tout ce qui se trouve sur la ligne avant l'appel à la sous-commande */
	size_t offsetLineCmd = line.find_first_of(ScriptSymbolsConstants::METHOD);
	string firstLinePart = (offsetLineCmd == std::string::npos ? "" : line.substr(0, offsetLineCmd));
	parsedArg += firstLinePart;

	do {
		string expression = line.substr(outputCommandSize);

		parsedArg += ScriptUtils::getFirstExpressionFromLine(expression, script, &offset);
		parsedArg += " ";
		outputCommandSize += offset;
	} while (offset != 0 && outputCommandSize < line.size());
	line = StringUtils::rtrim(parsedArg);
	return "";
}

AbstractCommand::~AbstractCommand()
{
}
