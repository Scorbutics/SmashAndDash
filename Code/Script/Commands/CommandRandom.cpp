#include "CommandRandom.h"
#include "../../Utils\ScriptUtils.h"
#include "../../Utils\StringUtils.h"

using namespace std;

CommandRandom::CommandRandom()
{
}


CommandRandom::~CommandRandom()
{
}

int CommandRandom::argumentsNumber() {
	return 1;
}

std::string CommandRandom::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	string s, value, commandCall;
	int valueInt, resultInt;

	value = args[0];
	valueInt = ska::StringUtils::strToInt(value);

	if (valueInt > 0)
	{
		resultInt = rand() % valueInt;
		return ska::StringUtils::intToStr(resultInt);
	}
	return "";
}
