#include "CommandElse.h"
#include "CommandIf.h"
#include "CommandElseEnd.h"
#include "../../Utils/StringUtils.h"
#include "../../Exceptions/ScriptSyntaxError.h"
#include <string>
#include <map>

using namespace std;


ska::CommandElse::CommandElse()
{
}


ska::CommandElse::~CommandElse()
{
}

const std::string& ska::CommandElse::getCmdName() {
	return ControlStatement::getCommandElse();
}

std::string ska::CommandElse::analyzeLine(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList)
{
	int ifEnd = 1;
	string lineBuf;

	while (ifEnd > 0 && !script->eof())
	{
		lineBuf = script->nextLine();
		ska::StringUtils::ltrim(lineBuf);
		if (lineBuf.find(ControlStatement::getCommandIf()) == 0)
			ifEnd++;
		else if (lineBuf == ControlStatement::getCommandEndIf())
			ifEnd--;
	}
	if (script->eof()) {
		throw ska::ScriptSyntaxError("Un endif est manquant");
	}

	return "";
}
