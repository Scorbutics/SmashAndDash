#pragma once
#include "AbstractFunctionCommand.h"
class CommandPlayShaking :
	public AbstractFunctionCommand
{
public:
	CommandPlayShaking();
	virtual ~CommandPlayShaking();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

