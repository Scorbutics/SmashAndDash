#include "CommandAssign.h"
#include <map>
#include "../../Utils\ScriptUtils.h"

using namespace std;

CommandAssign::CommandAssign()
{
}


CommandAssign::~CommandAssign()
{
}

int CommandAssign::argumentsNumber() {
	return 2;
}

bool CommandAssign::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	string valeur, varNumber, line, commandCall;
	size_t commandCallSize;

	valeur = args[0];
	varNumber = args[1];
	/*valeur = ScriptUtils::getFirstExpressionFromLine(valeur, extendedName, varMap, fscript, active, result, &commandCallSize);
	varNumber = ScriptUtils::getFirstExpressionFromLine(varNumber, extendedName, varMap, fscript, active, result, &commandCallSize);*/

	ScriptUtils::setValueFromVarOrSwitchNumber(extendedName, varNumber, valeur, varMap);
	return true;
}
