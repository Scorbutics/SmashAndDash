#pragma once
#include "AbstractFunctionCommand.h"
class CommandKillEntity :
	public AbstractFunctionCommand
{
public:
	CommandKillEntity();
	virtual ~CommandKillEntity();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

