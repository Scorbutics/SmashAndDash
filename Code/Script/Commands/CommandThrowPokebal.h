#pragma once
#include "AbstractFunctionCommand.h"
class CommandThrowPokebal :
	public AbstractFunctionCommand
{
public:
	CommandThrowPokebal();
	virtual ~CommandThrowPokebal();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

