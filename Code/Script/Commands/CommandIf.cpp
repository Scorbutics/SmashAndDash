#include "CommandIf.h"
#include <string>
#include <algorithm>
#include "../../Utils\StringUtils.h"

using namespace std;

CommandIf::CommandIf()
{
}


CommandIf::~CommandIf()
{
}

bool CommandIf::analyzeLine(const std::string& extendedName, std::string& lineArg, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	int ifEnd = 1, num1, num2;
	string varNumber, op, valeur, line;
	stringstream ss;
	ss << line;

	ss >> varNumber;
	ss >> op;
	ss >> valeur;

	num1 = StringUtils::strToInt(varNumber);
	num2 = StringUtils::strToInt(valeur);

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
