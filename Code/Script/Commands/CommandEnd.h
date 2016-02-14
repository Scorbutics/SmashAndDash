#pragma once
#include <iosfwd>
#include <map>
#include "AbstractFunctionCommand.h"

class CommandEnd : public AbstractFunctionCommand
{
public:
	CommandEnd();
	virtual ~CommandEnd();

	virtual bool execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override;
	virtual int argumentsNumber();
};

