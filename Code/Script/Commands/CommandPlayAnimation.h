#pragma once
#include "AbstractFunctionCommand.h"
class CommandPlayAnimation :
	public AbstractFunctionCommand
{
public:
	CommandPlayAnimation();
	virtual ~CommandPlayAnimation();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

