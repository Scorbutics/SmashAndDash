#pragma once
#include "AbstractFunctionCommand.h"
class CommandChoice :
	public AbstractFunctionCommand
{
public:
	CommandChoice();
	
	virtual ~CommandChoice();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

