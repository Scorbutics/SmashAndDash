#pragma once
#include "AbstractFunctionCommand.h"

class CommandLog :
	public AbstractFunctionCommand
{
public:
	CommandLog();
	virtual ~CommandLog();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

