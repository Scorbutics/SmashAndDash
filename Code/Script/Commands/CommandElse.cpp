#include "CommandElse.h"
#include "CommandIf.h"
#include "CommandElseEnd.h"
#include "../../Utils/StringUtils.h"
#include "../../Exceptions/ScriptSyntaxError.h"
#include <string>
#include <map>

using namespace std;


CommandElse::CommandElse()
{
}


CommandElse::~CommandElse()
{
}

const std::string& CommandElse::getCmdName() {
	return ControlStatement::getCommandElse();
}

bool CommandElse::analyzeLine(const std::string& extendedName, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	int ifEnd = 1;
	string lineBuf;

	while (ifEnd > 0 && !fscript.eof())
	{
		getline(fscript, lineBuf);
		StringUtils::ltrim(lineBuf);
		if (lineBuf.find(ControlStatement::getCommandIf()) == 0)
			ifEnd++;
		else if (lineBuf == ControlStatement::getCommandEndIf())
			ifEnd--;
	}
	if (fscript.eof()) {
		throw ScriptSyntaxError("[" + extendedName + "] Un endif est manquant");
	}

	return true;
}
