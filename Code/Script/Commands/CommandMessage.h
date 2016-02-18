#pragma once
#include "AbstractFunctionCommand.h"
class CommandMessage :
	public AbstractFunctionCommand
{
public:
	CommandMessage();
	
	virtual ~CommandMessage();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

