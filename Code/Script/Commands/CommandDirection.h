#pragma once
#include "AbstractFunctionCommand.h"
class CommandDirection :
	public AbstractFunctionCommand
{
public:
	CommandDirection();
	virtual ~CommandDirection();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

