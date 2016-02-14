#pragma once
#include <sstream>
#include "Command.h"

class AbstractCommand : public Command
{
public:
	AbstractCommand();
	bool execute(const std::string& extendedName, std::stringstream& streamCmd, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result);
	virtual ~AbstractCommand();

protected:
	virtual bool execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) = 0;
};

