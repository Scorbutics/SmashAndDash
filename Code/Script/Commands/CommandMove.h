#pragma once
#include "AbstractFunctionCommand.h"
class CommandMove :
	public AbstractFunctionCommand
{
public:
	CommandMove();
	
	virtual ~CommandMove();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

