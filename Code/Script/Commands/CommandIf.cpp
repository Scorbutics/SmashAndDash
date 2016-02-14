#include "CommandIf.h"
#include <string>
#include <algorithm>
#include "../../Utils\ScriptUtils.h"

using namespace std;

CommandIf::CommandIf()
{
}


CommandIf::~CommandIf()
{
}

int CommandIf::argumentsNumber() {
	return 3;
}

bool CommandIf::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	int ifEnd = 1, num1, num2;
	string varNumber, op, valeur, line;

	varNumber = args[0];
	op = args[1];
	valeur = args[2];

	num1 = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, varNumber, varMap);
	num2 = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, valeur, varMap);


	if (!((op == "==" && num1 == num2) || (op == "<" && num1 < num2) || (op == ">" && num1 > num2) || (op == "<=" && num1 <= num2) || (op == ">=" && num1 >= num2)))
	{

		while (ifEnd > 0 && getline(fscript, line))
		{
			stringstream ss;
			ss << line;
			ss >> line;
			line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

			if (line == "si")
				ifEnd++;
			else if (line == "finsi" || line == "sinon")
				ifEnd--;
		}

	}
	return true;
}
