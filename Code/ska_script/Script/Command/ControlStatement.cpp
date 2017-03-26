#include <iostream>
#include "ControlStatement.h"

ska::ControlStatement::ControlStatement(EntityManager& e) : AbstractCommand(e)
{
}

std::string ska::ControlStatement::process(ScriptComponent& script, std::stringstream& streamCmd, std::vector<std::string>& args) {
	return analyzeLine(script, streamCmd, args);
}

char ska::ControlStatement::getSeparator() {
	return ' ';
}

const std::string& ska::ControlStatement::getCommandEndIf() {
	static const std::string COMMAND_ENDIF = "endif";
	return COMMAND_ENDIF;
}

const std::string& ska::ControlStatement::getCommandElse() {
	static const std::string COMMAND_ELSE = "else";
	return COMMAND_ELSE;
}

const std::string& ska::ControlStatement::getCommandIf() {
	static const std::string COMMAND_IF = "if";
	return COMMAND_IF;
}

ska::ControlStatement::~ControlStatement()
{
}
