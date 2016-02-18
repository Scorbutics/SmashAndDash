#pragma once
#include "AbstractFunctionCommand.h"
class CommandAssign :
	public AbstractFunctionCommand
{
public:
	CommandAssign();
	virtual ~CommandAssign();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

