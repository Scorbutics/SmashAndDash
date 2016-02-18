#pragma once
#include "AbstractFunctionCommand.h"
class CommandCinematic :
	public AbstractFunctionCommand
{
public:
	CommandCinematic();
	virtual ~CommandCinematic();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

