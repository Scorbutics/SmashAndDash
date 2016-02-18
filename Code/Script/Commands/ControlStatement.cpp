#include <iostream>
#include "ControlStatement.h"

using namespace std;





ControlStatement::ControlStatement()
{
}

bool ControlStatement::process(const std::string& extendedName, std::stringstream& streamCmd, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) {
	return analyzeLine(extendedName, streamCmd, args, scriptList, varMap, fscript, active, result);
}

char ControlStatement::getSeparator() {
	return ' ';
}

const std::string& ControlStatement::getCommandEndIf() {
	static const std::string COMMAND_ENDIF = "endif";
	return COMMAND_ENDIF;
}

const std::string& ControlStatement::getCommandElse() {
	static const std::string COMMAND_ELSE = "else";
	return COMMAND_ELSE;
}

const std::string& ControlStatement::getCommandElsif() {
	static const std::string COMMAND_ELSIF = "elsif";
	return COMMAND_ELSIF;
}

const std::string& ControlStatement::getCommandIf() {
	static const std::string COMMAND_IF = "if";
	return COMMAND_IF;
}

ControlStatement::~ControlStatement()
{
}
