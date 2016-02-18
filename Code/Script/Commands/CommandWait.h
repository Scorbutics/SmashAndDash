#pragma once
#include "AbstractFunctionCommand.h"
class CommandWait :
	public AbstractFunctionCommand
{
public:
	CommandWait();
	virtual ~CommandWait();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

