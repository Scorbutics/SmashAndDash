#include <iostream>
#include "ControlStatement.h"

using namespace std;

ska::ControlStatement::ControlStatement()
{
}

std::string ska::ControlStatement::process(IScript* script, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList) {
	return analyzeLine(script, streamCmd, args, scriptList);
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

const std::string& ska::ControlStatement::getCommandElsif() {
	static const std::string COMMAND_ELSIF = "elsif";
	return COMMAND_ELSIF;
}

const std::string& ska::ControlStatement::getCommandIf() {
	static const std::string COMMAND_IF = "if";
	return COMMAND_IF;
}

ska::ControlStatement::~ControlStatement()
{
}
