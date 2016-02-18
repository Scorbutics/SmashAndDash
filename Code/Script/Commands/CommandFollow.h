#pragma once
#include "AbstractFunctionCommand.h"
class CommandFollow :
	public AbstractFunctionCommand
{
public:
	CommandFollow();
	virtual ~CommandFollow();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

