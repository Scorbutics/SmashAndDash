#include "CommandGet.h"
#include "../../Utils\ScriptUtils.h"
#include "../../Utils\StringUtils.h"

using namespace std;

CommandGet::CommandGet()
{
}


CommandGet::~CommandGet()
{
}

int CommandGet::argumentsNumber() {
	return 1;
}

bool CommandGet::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	string varNumber, line, commandCall;
	size_t commandCallSize;

	line = args[0];
	line = line.substr(1);
	if (line[0] == '[' && line[line.size() - 1] == ']') {

		varNumber = line.substr(1, line.size() - 2);

		if (result != NULL) {
			string& interpretedVar = ScriptUtils::interpretVarName(extendedName, varNumber, varMap);
			*result = StringUtils::intToStr(ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, interpretedVar.empty() ? line : interpretedVar, varMap));
		}
	}
	return true;
}
