#include "CommandElse.h"
#include <string>
#include <map>

using namespace std;

CommandElse::CommandElse()
{
}


CommandElse::~CommandElse()
{
}

int CommandElse::argumentsNumber() {
	return 0;
}

bool CommandElse::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	int ifEnd = 1;
	string line;

	while (ifEnd > 0 && getline(fscript, line))
	{
		if (line.find("si") == 0)
			ifEnd++;
		else if (line == "finsi")
			ifEnd--;
	}
	return true;
}
