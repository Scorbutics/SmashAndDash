#pragma once
#include "AbstractFunctionCommand.h"
class CommandScript :
	public AbstractFunctionCommand
{
public:
	CommandScript();
	virtual ~CommandScript();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

