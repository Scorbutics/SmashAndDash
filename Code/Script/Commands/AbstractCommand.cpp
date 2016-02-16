#include <iostream>
#include "AbstractCommand.h"
#include "..\..\Utils\StringUtils.h"
#include "..\..\Utils\ScriptUtils.h"
#include "../ScriptSymbolsConstants.h"

using namespace std;

AbstractCommand::AbstractCommand()
{
}

bool AbstractCommand::process(const string& extendedName, stringstream& streamCmd, ofstream& scriptList, unordered_map<string, string>& varMap, ifstream& fscript, int& active, string* result) {

	string line;
	getline(streamCmd, line);

	line = StringUtils::trim(line);

	/* Avant tout traitement, effectue toutes les exécutions des sous-commandes possibles */
	interpretSubCommands(line, extendedName, varMap, fscript, active, result);

	vector<string>& args = StringUtils::split(line, getSeparator());
	
	for (string& arg : args) {
		arg = StringUtils::trim(arg);
		/* Pour chaque argument, explicite les valeurs des variables */
		arg = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, arg, varMap);
	}
	

	return process(extendedName, streamCmd, args, scriptList, varMap, fscript, active, result);
}

void AbstractCommand::interpretSubCommands(string& line, const string& extendedName, unordered_map<string, string>& varMap, ifstream& fscript, int& active, string* result) {
	size_t outputCommandSize = 0;
	size_t offset;
	string parsedArg;
	
	/* Conservation de tout ce qui se trouve sur la ligne avant l'appel à la sous-commande */
	size_t offsetLineCmd = line.find_first_of(ScriptSymbolsConstants::METHOD);
	string firstLinePart = (offsetLineCmd == std::string::npos ? "" : line.substr(0, offsetLineCmd));
	parsedArg += firstLinePart;

	do {
		string expression = line.substr(outputCommandSize);

		parsedArg += ScriptUtils::getFirstExpressionFromLine(expression, extendedName, varMap, fscript, active, result, &offset);
		parsedArg += " ";
		outputCommandSize += offset;
	} while (offset != 0 && outputCommandSize < line.size());
	line = StringUtils::rtrim(parsedArg);
}

void AbstractCommand::parseArgument(std::string& arg, const string& extendedName, unordered_map<string, string>& varMap, ifstream& fscript, int& active, string* result) {

	
}

AbstractCommand::~AbstractCommand()
{
}
