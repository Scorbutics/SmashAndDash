#pragma once
#include "AbstractFunctionCommand.h"
#include <map>
class CommandHeal :
	public AbstractFunctionCommand
{
public:
	CommandHeal();
	virtual ~CommandHeal();

	virtual bool execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override;
	virtual int argumentsNumber();
};

