#include <iostream>
#include "CommandLog.h"

using namespace std;

CommandLog::CommandLog()
{
}

std::string CommandLog::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) {

	string& message = args[0];

	clog << message << endl;

	return "";
}

int CommandLog::argumentsNumber() {
	return 1;
}

CommandLog::~CommandLog()
{
}
