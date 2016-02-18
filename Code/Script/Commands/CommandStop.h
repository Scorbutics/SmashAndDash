#pragma once
#include "AbstractFunctionCommand.h"
class CommandStop :
	public AbstractFunctionCommand
{
public:
	CommandStop();
	~CommandStop();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

