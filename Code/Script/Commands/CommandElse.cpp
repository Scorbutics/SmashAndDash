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

bool CommandElse::analyzeLine(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	int ifEnd = 1;
	string lineBuf;

	while (ifEnd > 0 && getline(fscript, lineBuf))
	{
		if (lineBuf.find("si") == 0)
			ifEnd++;
		else if (lineBuf == "finsi")
			ifEnd--;
	}
	return true;
}
