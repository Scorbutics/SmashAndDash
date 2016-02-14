#include <iostream>
#include "AbstractCommand.h"
#include "..\..\Utils\StringUtils.h"

using namespace std;

char Command::ARGUMENT_SEPARATOR = ',';

AbstractCommand::AbstractCommand()
{
}

bool AbstractCommand::execute(const string& extendedName, stringstream& streamCmd, ofstream& scriptList, unordered_map<string, string>& varMap, ifstream& fscript, int& active, string* result) {
	int argNumber = argumentsNumber();

	string line;
	if (!getline(streamCmd, line)) {
		return false;
	} 

	vector<string>& args = StringUtils::split(line, Command::ARGUMENT_SEPARATOR);
	if (argNumber != args.size()) {
		/* Syntax error */
		//throw std::
		return false;
	}

	for (string& s : args) {
		s = StringUtils::trim(s);
	}

	return execute(extendedName, args, scriptList, varMap, fscript, active, result);
}

AbstractCommand::~AbstractCommand()
{
}
