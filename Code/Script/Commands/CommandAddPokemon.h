#pragma once
#include "AbstractFunctionCommand.h"
class CommandAddPokemon :
	public AbstractFunctionCommand
{
public:
	CommandAddPokemon();
	virtual ~CommandAddPokemon();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

