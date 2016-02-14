#pragma once
#include "AbstractCommand.h"
class CommandPlayAnimation :
	public AbstractCommand
{
public:
	CommandPlayAnimation();
	virtual ~CommandPlayAnimation();

	virtual bool execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override;
	virtual int argumentsNumber();
};

