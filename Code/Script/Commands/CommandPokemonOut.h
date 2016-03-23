#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandPokemonOut :
	public ska::AbstractFunctionCommand
{
public:
	CommandPokemonOut();
	virtual ~CommandPokemonOut();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

