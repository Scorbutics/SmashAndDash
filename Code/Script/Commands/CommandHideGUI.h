#pragma once
#include "AbstractCommand.h"
class CommandHideGUI :
	public AbstractCommand
{
public:
	CommandHideGUI();
	virtual ~CommandHideGUI();

	virtual bool execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result) override;
	virtual int argumentsNumber();
};
