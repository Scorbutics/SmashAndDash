#include <iostream>
#include "CommandLog.h"

using namespace std;

ska::CommandLog::CommandLog()
{
}

std::string ska::CommandLog::execute(ScriptComponent& script, std::vector<std::string>& args) {

	string& message = args[0];

	clog << message << endl;

	return "";
}

int ska::CommandLog::argumentsNumber() {
	return 1;
}

ska::CommandLog::~CommandLog()
{
}
