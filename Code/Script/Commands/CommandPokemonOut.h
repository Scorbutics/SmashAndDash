#pragma once
#include "AbstractFunctionCommand.h"
class CommandPokemonOut :
	public AbstractFunctionCommand
{
public:
	CommandPokemonOut();
	virtual ~CommandPokemonOut();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

