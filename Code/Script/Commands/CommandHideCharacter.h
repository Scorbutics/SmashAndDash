#pragma once
#include "AbstractFunctionCommand.h"
class CommandHideCharacter :
	public AbstractFunctionCommand
{
public:
	CommandHideCharacter();
	virtual ~CommandHideCharacter();

	virtual bool execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override;
	virtual int argumentsNumber();
};

