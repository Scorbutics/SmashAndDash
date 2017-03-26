#include "CommandIf.h"
#include <string>
#include <algorithm>
#include "Utils/StringUtils.h"
#include "Exceptions/ScriptSyntaxError.h"
#include "../System/ScriptAutoSystem.h"

ska::CommandIf::CommandIf(EntityManager& e) : ControlStatement(e)
{
}


ska::CommandIf::~CommandIf()
{
}

const std::string& ska::CommandIf::getCmdName() {
	return getCommandIf();
}

std::string ska::CommandIf::analyzeLine(ScriptComponent& script, std::stringstream&, std::vector<std::string>& args) {
	int ifEnd = 1, num1, num2;
	std::string varNumber, op, valeur, line;
	std::stringstream ss;

	varNumber = args[0];
	op = args[1];
	valeur = args[2];

	num1 = StringUtils::strToInt(varNumber);
	num2 = StringUtils::strToInt(valeur);

	if (!((op == "==" && num1 == num2) || (op == "<" && num1 < num2) || (op == ">" && num1 > num2) || (op == "<=" && num1 <= num2) || (op == ">=" && num1 >= num2)))
	{

		while (ifEnd > 0 && !script.parent->eof(script))
		{
			line = script.parent->nextLine(script);
			std::stringstream ss;
			ss << line;
			ss >> line;
			StringUtils::trim(line);

			if (line == getCommandIf())
				ifEnd++;
			else if (line == getCommandEndIf() || line == getCommandElse())
				ifEnd--;
		}

		if (script.parent->eof(script)) {
			throw ScriptSyntaxError("Un " + getCommandEndIf() + " est manquant");
		}

	}

	return "";
}
