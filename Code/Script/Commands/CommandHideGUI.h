#pragma once
#include "AbstractFunctionCommand.h"
class CommandHideGUI :
	public AbstractFunctionCommand
{
public:
	CommandHideGUI();
	virtual ~CommandHideGUI();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

