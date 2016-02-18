#pragma once
#include "AbstractFunctionCommand.h"
class CommandGet :
	public AbstractFunctionCommand
{
public:
	CommandGet();
	virtual ~CommandGet();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

