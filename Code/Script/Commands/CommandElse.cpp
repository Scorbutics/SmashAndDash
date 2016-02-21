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

std::string CommandElse::analyzeLine(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList)
{
	int ifEnd = 1;
	string lineBuf;

	while (ifEnd > 0 && !script->eof())
	{
		lineBuf = script->nextLine();
		StringUtils::ltrim(lineBuf);
		if (lineBuf.find(ControlStatement::getCommandIf()) == 0)
			ifEnd++;
		else if (lineBuf == ControlStatement::getCommandEndIf())
			ifEnd--;
	}
	if (script->eof()) {
		throw ScriptSyntaxError("Un endif est manquant");
	}

	return "";
}
