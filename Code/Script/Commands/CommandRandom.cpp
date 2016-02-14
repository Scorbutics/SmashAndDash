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

bool CommandRandom::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	string s, value, commandCall;
	int valueInt, resultInt;

	value = args[0];
	valueInt = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, value, varMap);

	if (valueInt == -1)
		valueInt = atoi(value.c_str());

	if (valueInt > 0)
	{
		resultInt = rand() % valueInt;
		if (result != NULL)
			*result = StringUtils::intToStr(resultInt);
	}
	return true;
}
