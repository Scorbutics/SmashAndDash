#include "CommandIf.h"
#include <string>
#include <algorithm>
#include "../../Utils\StringUtils.h"
#include "../../Exceptions/ScriptSyntaxError.h"

using namespace std;

ska::CommandIf::CommandIf()
{
}


ska::CommandIf::~CommandIf()
{
}

const string& ska::CommandIf::getCmdName() {
	return ControlStatement::getCommandIf();
}

std::string ska::CommandIf::analyzeLine(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList)
{
	int ifEnd = 1, num1, num2;
	string varNumber, op, valeur, line;
	stringstream ss;

	varNumber = args[0];
	op = args[1];
	valeur = args[2];

	num1 = ska::StringUtils::strToInt(varNumber);
	num2 = ska::StringUtils::strToInt(valeur);

	if (!((op == "==" && num1 == num2) || (op == "<" && num1 < num2) || (op == ">" && num1 > num2) || (op == "<=" && num1 <= num2) || (op == ">=" && num1 >= num2)))
	{

		while (ifEnd > 0 && !script->eof())
		{
			line = script->nextLine();
			stringstream ss;
			ss << line;
			ss >> line;
			ska::StringUtils::trim(line);

			if (line == ControlStatement::getCommandIf())
				ifEnd++;
			else if (line == ControlStatement::getCommandEndIf() || line == ControlStatement::getCommandElse() || line == ControlStatement::getCommandElsif())
				ifEnd--;
		}

		if (script->eof()) {
			throw ska::ScriptSyntaxError("Un " + ControlStatement::getCommandEndIf() + " est manquant");
		}

	}

	return "";
}
