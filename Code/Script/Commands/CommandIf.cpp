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

const string& CommandIf::getCmdName() {
	return ControlStatement::getCommandIf();
}

bool CommandIf::analyzeLine(const std::string& extendedName, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	int ifEnd = 1, num1, num2;
	string varNumber, op, valeur, line;
	stringstream ss;

	varNumber = args[0];
	op = args[1];
	valeur = args[2];

	num1 = StringUtils::strToInt(varNumber);
	num2 = StringUtils::strToInt(valeur);

	if (!((op == "==" && num1 == num2) || (op == "<" && num1 < num2) || (op == ">" && num1 > num2) || (op == "<=" && num1 <= num2) || (op == ">=" && num1 >= num2)))
	{

		while (ifEnd > 0 && !fscript.eof())
		{
			getline(fscript, line);
			stringstream ss;
			ss << line;
			ss >> line;
			StringUtils::trim(line);

			if (line == ControlStatement::getCommandIf())
				ifEnd++;
			else if (line == ControlStatement::getCommandEndIf() || line == ControlStatement::getCommandElse() || line == ControlStatement::getCommandElsif())
				ifEnd--;
		}


	}
	return true;
}
