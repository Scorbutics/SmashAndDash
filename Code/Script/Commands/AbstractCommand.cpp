#include <iostream>
#include "AbstractCommand.h"
#include "..\..\Utils\StringUtils.h"
#include "..\..\Utils\ScriptUtils.h"

using namespace std;

char Command::ARGUMENT_SEPARATOR = ',';

AbstractCommand::AbstractCommand()
{
}

bool AbstractCommand::process(const string& extendedName, stringstream& streamCmd, ofstream& scriptList, unordered_map<string, string>& varMap, ifstream& fscript, int& active, string* result) {

	string line;
	if (!getline(streamCmd, line)) {
		return false;
	}

	vector<string>& args = StringUtils::split(line, getSeparator());

	/* Pour chaque argument, effectue toutes les opérations de calcul nécessaires et explicite les valeurs des variables */
	for (string& arg : args) {
		arg = StringUtils::trim(arg);
		parseArgument(arg, extendedName, varMap, fscript, active, result);
	}
	

	return process(extendedName, args, scriptList, varMap, fscript, active, result);
}


void AbstractCommand::parseArgument(std::string& arg, const string& extendedName, unordered_map<string, string>& varMap, ifstream& fscript, int& active, string* result) {
	size_t outputCommandSize = 0;
	size_t offset;
	string parsedArg;
	do {
		string expression = arg.substr(outputCommandSize);

		parsedArg += ScriptUtils::getFirstExpressionFromLine(expression, extendedName, varMap, fscript, active, result, &offset);
		parsedArg += " ";
		outputCommandSize += offset;
	} while (offset != 0 && outputCommandSize < arg.size());
	arg = StringUtils::rtrim(parsedArg);
	arg = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, arg, varMap);
}

AbstractCommand::~AbstractCommand()
{
}
