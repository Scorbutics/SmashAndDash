#include <string>
#include "CommandElse.h"
#include "CommandIf.h"
#include "CommandElseEnd.h"
#include "../../Utils/StringUtils.h"
#include "../../Exceptions/ScriptSyntaxError.h"
#include "../ScriptComponent.h"
#include "../System/ScriptAutoSystem.h"


ska::CommandElse::CommandElse(EntityManager& entityManager) : ControlStatement(entityManager)
{
}


ska::CommandElse::~CommandElse()
{
}

const std::string& ska::CommandElse::getCmdName() {
	return ControlStatement::getCommandElse();
}

std::string ska::CommandElse::analyzeLine(ScriptComponent& script, std::stringstream& streamCmd, std::vector<std::string>& args) {
	int ifEnd = 1;
	std::string lineBuf;

	while (ifEnd > 0 && !script.parent->eof(script)) {
		lineBuf = script.parent->nextLine(script);
		ska::StringUtils::ltrim(lineBuf);
		if (lineBuf.find(ControlStatement::getCommandIf()) == 0)
			ifEnd++;
		else if (lineBuf == ControlStatement::getCommandEndIf())
			ifEnd--;
	}

	if (script.parent->eof(script)) {
		throw ska::ScriptSyntaxError("Un endif est manquant");
	}

	return "";
}
