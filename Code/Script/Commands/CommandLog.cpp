#include <iostream>
#include "CommandLog.h"

using namespace std;

CommandLog::CommandLog()
{
}

bool CommandLog::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) {

	string& message = args[0];

	clog << message << endl;

	return true;
}

int CommandLog::argumentsNumber() {
	return 1;
}

CommandLog::~CommandLog()
{
}
