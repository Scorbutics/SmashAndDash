#pragma once
#include "AbstractFunctionCommand.h"
class CommandRandom :
	public AbstractFunctionCommand
{
public:
	CommandRandom();
	virtual ~CommandRandom();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

